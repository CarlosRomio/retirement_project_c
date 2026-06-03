#pragma once
#include "domain/repositories/user-repository.hpp"

class InitializeAdminUseCase {
private:
	IUserRepository* userRepository;

public:
	InitializeAdminUseCase(IUserRepository& repo);

	// Garante que admin seed existe no banco
	bool ensureAdminExists();
};
