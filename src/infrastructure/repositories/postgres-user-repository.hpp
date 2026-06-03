#pragma once

#include <optional>

#include "domain/repositories/user-repository.hpp"
#include "database/connection/database-connection.hpp"

class PostgresUserRepository : public IUserRepository {
private:
    DatabaseConnection& db;

public:
    PostgresUserRepository(DatabaseConnection& db);

    void save(const User& user) override;
    std::optional<User> findById(const std::string& id) override;
    void update(const User& user) override;
};
