#pragma once
#include <drogon/HttpController.h>

class CorsController : public drogon::HttpController<CorsController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(CorsController::handleCors, "/{path:.*}", drogon::Options);
    METHOD_LIST_END

    void handleCors(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        const std::string& path
    );
};
