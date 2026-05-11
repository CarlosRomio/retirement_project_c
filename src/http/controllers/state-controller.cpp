#include "state-controller.hpp"

crow::response StateController::get_state(const crow::request&) {
    crow::json::wvalue json_response;
    json_response["status"] = "ok";
    json_response["message"] = "API rodando";
    json_response["timestamp"] = std::time(nullptr);

    auto res = crow::response(json_response);
    res.add_header("Access-Control-Allow-Origin", "*");
    res.set_header("Content-Type", "application/json; charset=utf-8");
    return res;
}