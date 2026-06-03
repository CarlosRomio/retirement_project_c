#include "initialize-admin-use-case.hpp"
#include "core/auth/auth-permission.hpp"
#include "database/seeds/admin-seed.hpp"

InitializeAdminUseCase::InitializeAdminUseCase(IUserRepository& repo)
	: userRepository(&repo) {}

bool InitializeAdminUseCase::ensureAdminExists() {
	try {
		// Verifica se admin já existe
		auto existingAdmin = userRepository->findById(seeds::ADMIN_SEED_DATA.id);
		if (existingAdmin) {
			return true;  // Já existe
		}

		// Cria admin seed
		User adminUser(
			seeds::ADMIN_SEED_DATA.id,
			seeds::ADMIN_SEED_DATA.name,
			seeds::ADMIN_SEED_DATA.email,
			auth::Role::ADMIN
		);

		userRepository->save(adminUser);
		return true;
	}
	catch (const std::exception&) {
		// Se falhar (email já existe), ignora
		return false;
	}
}
