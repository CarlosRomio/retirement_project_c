#pragma once
#include "crow.h"

class StateController {
public:
    static crow::response get_state(const crow::request& req);
};