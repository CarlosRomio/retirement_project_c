#include "web-server-service.hpp"
#include <drogon/drogon.h>

WebServerService::WebServerService(AppContext& ctx)
    : context(ctx) {}

void WebServerService::start() {
    drogon::app()
        .addListener("0.0.0.0", 8080)
        .setThreadNum(4);

    std::cout << "Servidor rodando em http://localhost:8080\n";
    drogon::app().run();
}
