#include "migration-runner.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <set>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;

MigrationRunner::MigrationRunner(const std::string& migrations_path)
    : path(migrations_path) {}

std::string MigrationRunner::load_file(const std::string& filepath) {
    std::ifstream file(filepath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void MigrationRunner::run(pqxx::connection& conn) {
    pqxx::work txn(conn);

    // 1. garante tabela de controle
    txn.exec(R"(
        CREATE TABLE IF NOT EXISTS migrations (
            id TEXT PRIMARY KEY,
            executed_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        );
    )");

    // 2. pega migrations já executadas
    std::set<std::string> executed;

    auto res = txn.exec("SELECT id FROM migrations");
    for (auto row : res) {
        executed.insert(row["id"].c_str());
    }

    // 3. lista arquivos do diretório
    std::vector<std::string> files;

    for (const auto& entry : fs::directory_iterator(path)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path().filename().string());
        }
    }

    // 4. ordena (IMPORTANTE)
    std::sort(files.begin(), files.end());

    // 5. executa o que falta
    for (const auto& file : files) {
        if (executed.count(file)) {
            continue;
        }

        std::string full_path = path + "/" + file;
        std::string sql = load_file(full_path);

        txn.exec(sql);

        txn.exec_params(
            "INSERT INTO migrations (id) VALUES ($1)",
            file
        );
    }

    txn.commit();
}