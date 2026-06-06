#include "cors-middleware.hpp"

drogon::HttpResponsePtr CorsMiddleware::handleOptions(const drogon::HttpRequestPtr& req) {
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k200OK);
    addCorsHeaders(req, resp);
    return resp;
}

void CorsMiddleware::addCorsHeaders(const drogon::HttpRequestPtr& req, drogon::HttpResponsePtr& resp) {
    resp->addHeader("Access-Control-Allow-Origin", "*");
    resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS, PATCH");
    resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization, x-user-id");
    resp->addHeader("Access-Control-Max-Age", "86400");
    resp->addHeader("Access-Control-Allow-Credentials", "false");
}

