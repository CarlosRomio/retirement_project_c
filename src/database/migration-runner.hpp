#pragma once

#include <pqxx/pqxx>
#include <string>

class MigrationRunner {
public:
    explicit MigrationRunner(const std::string& migrations_path);

    void run(pqxx::connection& conn);

private:
    std::string path;

    std::string load_file(const std::string& filepath);
};