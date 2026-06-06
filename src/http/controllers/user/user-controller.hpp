#pragma once
#include <drogon/HttpController.h>
#include "core/app-context.hpp"

class UserController : public drogon::HttpController<UserController> {
public:
    UserController();

    METHOD_LIST_BEGIN
        ADD_METHOD_TO(UserController::create, "/api/users", drogon::Post);
        ADD_METHOD_TO(UserController::me, "/api/users/me", drogon::Get);
        ADD_METHOD_TO(UserController::updateMe, "/api/users/me", drogon::Put);
        ADD_METHOD_TO(UserController::options, "/api/users", drogon::Options);
        ADD_METHOD_TO(UserController::options, "/api/users/me", drogon::Options);
    METHOD_LIST_END

        void create(
            const drogon::HttpRequestPtr& req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback
        );

    void me(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

    void updateMe(
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