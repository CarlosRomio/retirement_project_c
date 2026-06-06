#pragma once
#include <drogon/plugins/Plugin.h>
#include <drogon/HttpFilter.h>

class CorsFilter : public drogon::HttpFilter<CorsFilter> {
public:
    virtual void doFilter(
        const drogon::HttpRequestPtr& req,
        drogon::FilterCallback&& fcb,
        drogon::FilterChainCallback&& fccb
    ) override;
};
