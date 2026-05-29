#include "postgres-user-repository.hpp"
#include <pqxx/pqxx>

PostgresUserRepository::PostgresUserRepository(DatabaseConnection& db)
    : db(db) {}

void PostgresUserRepository::save(const User& user) {
    pqxx::work txn(db.get());

    try {
        txn.exec_params(
            "INSERT INTO users (id, name, email) VALUES ($1, $2, $3)",
            user.get_id(),
            user.get_name(),
            user.get_email()
        );

        txn.commit();
    }
    catch (const pqxx::unique_violation&) {
        throw std::runtime_error("User with this email already exists");
    }
    catch (const pqxx::sql_error& e) {
        throw std::runtime_error(std::string("Database error: ") + e.what());
    }
}

std::optional<User> PostgresUserRepository::findById(const std::string& id) {
    try {
        pqxx::work txn(db.get());

        auto result = txn.exec_params(
            "SELECT id, name, email FROM users WHERE id = $1",
            id
        );

        if (result.empty()) {
            return std::nullopt;
        }

        const auto& row = result[0];

        return User(
            row["id"].c_str(),
            row["name"].c_str(),
            row["email"].c_str()
        );
    }
    catch (const pqxx::sql_error& e) {
        throw std::runtime_error(std::string("Database error: ") + e.what());
    }
}

void PostgresUserRepository::update(const User& user) {
    pqxx::work txn(db.get());

    txn.exec_params(
        "UPDATE users SET name = $1 WHERE id = $2",
        user.get_name(),
        user.get_id()
    );

    txn.commit();
}