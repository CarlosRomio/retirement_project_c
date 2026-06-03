#pragma once
#include "login-dto.hpp"
#include "domain/repositories/user-repository.hpp"

class LoginUseCase {
private:
	IUserRepository* userRepository;

public:
	LoginUseCase(IUserRepository& repo);

	LoginOutput execute(const LoginInput& input);
};
