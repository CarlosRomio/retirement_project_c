#pragma once
#include <string>

struct CreateUserInput {
    std::string id;
    std::string name;
    std::string email;
};

struct CreateUserOutput {
    std::string id;
};