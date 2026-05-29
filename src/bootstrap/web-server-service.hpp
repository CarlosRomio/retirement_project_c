#pragma once

#include "core/app-context.hpp"

class WebServerService {
public:
    explicit WebServerService(AppContext& ctx);

    void start();

private:
    AppContext& context;

};