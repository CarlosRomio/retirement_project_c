#pragma once
#include <string>

struct CreateUserInput {
    std::string name;
    std::string email;
    std::string password;
};

struct CreateUserOutput {
    std::string id;
};