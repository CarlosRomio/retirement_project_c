#pragma once
#include <drogon/HttpController.h>
#include "core/app-context.hpp"

class AuthController : public drogon::HttpController<AuthController> {
public:
    AuthController();

    METHOD_LIST_BEGIN
        ADD_METHOD_TO(AuthController::login, "/api/auth/login", drogon::Post);
        ADD_METHOD_TO(AuthController::options, "/api/auth/login", drogon::Options);
    METHOD_LIST_END

    void login(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

    void options(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

private:
    AppContext& context;
};
