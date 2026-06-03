#pragma once

#include "application/use-cases/create-task/create-task-use-case.hpp"
#include "application/use-cases/create-trip/create-trip-use-case.hpp"
#include "application/use-cases/create-user/create-user-use-case.hpp"
#include "application/use-cases/delete-task/delete-task-use-case.hpp"
#include "application/use-cases/delete-trip/delete-trip-use-case.hpp"
#include "application/use-cases/get-task/get-task-use-case.hpp"
#include "application/use-cases/get-trip/get-trip-use-case.hpp"
#include "application/use-cases/login/login-use-case.hpp"
#include "application/use-cases/initialize/initialize-admin-use-case.hpp"
#include "database/connection/database-connection.hpp"
#include "infrastructure/repositories/postgres-task-repository.hpp"
#include "infrastructure/repositories/postgres-trip-repository.hpp"
#include "infrastructure/repositories/postgres-user-repository.hpp"

class AppContext {
public:
    DatabaseConnection db;
    PostgresUserRepository userRepository;
    PostgresTripRepository tripRepository;
    PostgresTaskRepository taskRepository;

    CreateUserUseCase createUserUseCase;
    CreateTripUseCase createTripUseCase;
    GetTripUseCase getTripUseCase;
    DeleteTripUseCase deleteTripUseCase;

    CreateTaskUseCase createTaskUseCase;
    GetTaskUseCase getTaskUseCase;
    DeleteTaskUseCase deleteTaskUseCase;

    LoginUseCase loginUseCase;
    InitializeAdminUseCase initializeAdminUseCase;

    AppContext(const std::string& conn_str);
};
