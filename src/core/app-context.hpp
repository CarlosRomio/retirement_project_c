#pragma once

#include "database/connection/database-connection.hpp"
#include "infrastructure/repositories/postgres-user-repository.hpp"
#include "application/use-cases/create-user/create-user-use-case.hpp"

class AppContext {
public:
    DatabaseConnection db;
    PostgresUserRepository userRepository;
    CreateUserUseCase createUserUseCase;

    AppContext(const std::string& conn_str);
};