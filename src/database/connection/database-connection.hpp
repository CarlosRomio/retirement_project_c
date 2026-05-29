#pragma once
#include <pqxx/pqxx>
#include <memory>
#include <string>

class DatabaseConnection {
private:
    std::unique_ptr<pqxx::connection> conn;

public:
    DatabaseConnection(const std::string& conn_str);

    pqxx::connection& get();
};