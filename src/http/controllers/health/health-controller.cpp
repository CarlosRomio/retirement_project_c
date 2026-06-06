#include "health-controller.hpp"
#include <json/json.h>
#include "http/controllers/utils/logging.hpp"

void HealthController::state(
    const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    logging::logRequest("GET", "/api/state");

    Json::Value res;
    res["status"] = "ok";

    auto resp = drogon::HttpResponse::newHttpJsonResponse(res);
    callback(resp);
}

void HealthController::options(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k200OK);
    resp->addHeader("Access-Control-Allow-Origin", "*");
    resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS, PATCH");
    resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization, x-user-id");
    resp->addHeader("Access-Control-Max-Age", "86400");
    callback(resp);
}
