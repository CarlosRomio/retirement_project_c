#include "login-use-case.hpp"
#include "core/auth/auth-permission.hpp"

LoginUseCase::LoginUseCase(IUserRepository& repo)
	: userRepository(&repo) {}

LoginOutput LoginUseCase::execute(const LoginInput& input) {
	// Validações básicas
	if (input.email.empty()) {
		throw std::invalid_argument("Email cannot be empty");
	}

	if (input.password.empty()) {
		throw std::invalid_argument("Password cannot be empty");
	}

	// Busca usuário por email
	auto userOpt = userRepository->findByEmail(input.email);

	if (!userOpt) {
		throw std::invalid_argument("Invalid email or password");
	}

	const auto& user = *userOpt;

	// TODO: Validar password hash quando integrar com Firebase/Supabase
	// Por enquanto, apenas retornamos o usuário se existir
	// Em produção: if (!validatePasswordHash(input.password, user.password_hash))

	LoginOutput output;
	output.user_id = user.get_id();
	output.name = user.get_name();
	output.email = user.get_email();
	output.role = auth::roleToString(user.get_role());

	return output;
}
