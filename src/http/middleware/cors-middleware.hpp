#pragma once
#include <drogon/HttpController.h>

class CorsMiddleware {
public:
    static drogon::HttpResponsePtr handleOptions(const drogon::HttpRequestPtr& req);
    static void addCorsHeaders(const drogon::HttpRequestPtr& req, drogon::HttpResponsePtr& resp);
};

