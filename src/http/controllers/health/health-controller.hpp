#pragma once
#include <drogon/HttpController.h>

class HealthController : public drogon::HttpController<HealthController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(HealthController::state, "/api/state", drogon::Get);
    METHOD_LIST_END

        void state(const drogon::HttpRequestPtr&,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};