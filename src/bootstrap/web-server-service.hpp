#pragma once

#include "core/app-context.hpp"
#include "http/middleware/cors-middleware.hpp"

class WebServerService {
public:
    explicit WebServerService(AppContext& ctx);

    void start();

private:
    AppContext& context;

};