#pragma once
#include <drogon/HttpController.h>
#include "core/auth/auth-extractor.hpp"
#include "core/auth/auth-user.hpp"

namespace responses {
    inline drogon::HttpResponsePtr error(
        drogon::HttpStatusCode statusCode,
        const std::string& message
    ) {
        Json::Value body;
        body["error"] = message;

        auto response = drogon::HttpResponse::newHttpJsonResponse(body);
        response->setStatusCode(statusCode);
        return response;
    }

    inline drogon::HttpResponsePtr badRequest(const std::string& msg) {
        return error(drogon::k400BadRequest, msg);
    }

    inline drogon::HttpResponsePtr unauthorized(const std::string& msg = "Unauthorized") {
        return error(drogon::k401Unauthorized, msg);
    }

    inline drogon::HttpResponsePtr notFound(const std::string& msg) {
        return error(drogon::k404NotFound, msg);
    }

    inline drogon::HttpResponsePtr conflict(const std::string& msg) {
        return error(drogon::k409Conflict, msg);
    }

    inline drogon::HttpResponsePtr internalServerError(const std::string& msg = "Internal server error") {
        return error(drogon::k500InternalServerError, msg);
    }
}

namespace auth {
    inline std::optional<AuthUser> requireAuth(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>& callback
    ) {
        auto user = AuthExtractor::extract(req);

        if (user.id.empty()) {
            callback(responses::unauthorized("Missing authentication headers"));
            return std::nullopt;
        }

        return user;
    }

    inline AuthUser getAuthUser(const drogon::HttpRequestPtr& req) {
        return req->attributes()->get<AuthUser>("authUser");
    }
}
