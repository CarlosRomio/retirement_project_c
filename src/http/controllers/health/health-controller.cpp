#include "health-controller.hpp"

void HealthController::state(
    const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    auto resp = drogon::HttpResponse::newHttpJsonResponse({
        {"status", "ok"}
        });

    callback(resp);
}