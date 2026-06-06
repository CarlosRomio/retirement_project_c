#include "cors-controller.hpp"
#include "http/middleware/cors-middleware.hpp"

void CorsController::handleCors(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    const std::string& path
) {
    callback(CorsMiddleware::handleOptions(req));
}
