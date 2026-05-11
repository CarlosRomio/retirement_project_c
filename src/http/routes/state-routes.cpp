// state-routes.cpp
#include "state-routes.hpp"
#include "http/controllers/state-controller.hpp"

void register_state_routes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/api/state").methods("GET"_method)(
        &StateController::get_state
        );
}