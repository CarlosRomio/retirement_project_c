#include "user-controller.hpp"
#include "application/use-cases/create-user-use-case.hpp"
//#include "infrastructure/repositories/in-memory-user-repository.hpp"

crow::response UserController::create(const crow::request& req) {
    auto body = crow::json::load(req.body);

    if (!body) {
        return crow::response(400, "Invalid JSON");
    }

    CreateUserInput input;
    input.name = body["name"].s();
    input.email = body["email"].s();
    input.password = body["password"].s();

    //InMemoryUserRepository repo;
    //CreateUserUseCase useCase(&repo);

    //auto result = useCase.execute(input);

    crow::json::wvalue res;
    //res["id"] = result.id;

    return crow::response(201, res);
}