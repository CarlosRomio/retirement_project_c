#pragma once

#include "application/use-cases/create-trip/create-trip-use-case.hpp"
#include "application/use-cases/create-user/create-user-use-case.hpp"
#include "application/use-cases/delete-trip/delete-trip-use-case.hpp"
#include "application/use-cases/get-trip/get-trip-use-case.hpp"
#include "database/connection/database-connection.hpp"
#include "infrastructure/repositories/postgres-trip-repository.hpp"
#include "infrastructure/repositories/postgres-user-repository.hpp"

class AppContext {
public:
    DatabaseConnection db;
    PostgresUserRepository userRepository;
    PostgresTripRepository tripRepository;

    CreateUserUseCase createUserUseCase;
    CreateTripUseCase createTripUseCase;
    GetTripUseCase getTripUseCase;
    DeleteTripUseCase deleteTripUseCase;

    AppContext(const std::string& conn_str);
};
