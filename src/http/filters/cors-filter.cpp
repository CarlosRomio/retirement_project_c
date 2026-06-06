#include "cors-filter.hpp"

void CorsFilter::doFilter(
    const drogon::HttpRequestPtr& req,
    drogon::FilterCallback&& fcb,
    drogon::FilterChainCallback&& fccb
) {
    // Handle preflight requests
    if (req->getMethod() == drogon::HttpMethod::Options) {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k200OK);
        resp->addHeader("Access-Control-Allow-Origin", "*");
        resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS, PATCH");
        resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization, x-user-id");
        resp->addHeader("Access-Control-Max-Age", "86400");
        fcb(resp);
        return;
    }

    // Continue to next filter/handler
    fccb();
}
