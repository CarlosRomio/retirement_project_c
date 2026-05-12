#include "user-routes.hpp"
#include "../controllers/user-controller.hpp"

void register_user_routes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/api/users").methods("POST"_method)(
        &UserController::create
        );
}