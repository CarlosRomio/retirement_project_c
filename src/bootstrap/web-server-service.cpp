#include "web-server-service.hpp"
#include <drogon/drogon.h>

WebServerService::WebServerService(AppContext& ctx)
    : context(ctx) {}

void WebServerService::start() {
    auto& app = drogon::app();

    // Add CORS headers to all responses
    app.registerPostHandlingAdvice([](const drogon::HttpRequestPtr& req, const drogon::HttpResponsePtr& resp) {
        if (resp) {
            const_cast<drogon::HttpResponsePtr&>(resp)->addHeader("Access-Control-Allow-Origin", "*");
            const_cast<drogon::HttpResponsePtr&>(resp)->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS, PATCH");
            const_cast<drogon::HttpResponsePtr&>(resp)->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization, x-user-id");
            const_cast<drogon::HttpResponsePtr&>(resp)->addHeader("Access-Control-Max-Age", "86400");
        }
    });

    app.addListener("0.0.0.0", 8080)
        .setThreadNum(4);

    std::cout << "Servidor rodando em http://localhost:8080\n";
    app.run();
}



