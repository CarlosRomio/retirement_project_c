#pragma once

#include <drogon/HttpFilter.h>
#include "http/controllers/utils/http-helper.hpp"

class AuthFilter : public drogon::HttpFilter<AuthFilter> {
public:
    void doFilter(
        const drogon::HttpRequestPtr& req,
        drogon::FilterCallback&& fcb,
        drogon::FilterChainCallback&& fccb
    ) override {
        auto user = AuthExtractor::extract(req);

        if (user.id.empty()) {
            fcb(responses::unauthorized("Missing authentication headers"));
            return;
        }

        req->attributes()->insert("authUser", user);
        fccb();
    }
};
