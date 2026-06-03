#pragma once
#include <string>

struct LoginInput {
	std::string email;
	std::string password;
};

struct LoginOutput {
	std::string user_id;
	std::string name;
	std::string email;
	std::string role;
};
