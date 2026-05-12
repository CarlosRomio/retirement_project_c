#pragma once
#include "create-user-dto.hpp"
#include "domain/entities/user.hpp"

class IUserRepository;

class CreateUserUseCase {
private:
    IUserRepository* userRepository;

public:
    CreateUserUseCase(IUserRepository* repo);

    CreateUserOutput execute(const CreateUserInput& input);
};