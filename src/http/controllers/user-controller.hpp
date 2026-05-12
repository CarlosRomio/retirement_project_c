#pragma once
#include "crow.h"

class UserController {
public:
    static crow::response create(const crow::request& req);
};