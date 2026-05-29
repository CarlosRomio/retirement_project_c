#pragma once
#include "auth-user.hpp"
#include <drogon/HttpRequest.h>

class AuthExtractor {
public:
    static AuthUser extract(const drogon::HttpRequestPtr& req) {
        AuthUser user;
        user.id = req->getHeader("x-user-id");

        if (user.id.empty()) {
            return {};
        }

        return user;
    }
};
