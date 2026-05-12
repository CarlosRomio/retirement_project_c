#include "create-user-use-case.hpp"
#include "infrastructure/repositories/user-repository.hpp"
#include <random>

std::string generate_id() {
    return std::to_string(std::rand()); // simples por agora
}

CreateUserUseCase::CreateUserUseCase(IUserRepository* repo)
    : userRepository(repo) {}

CreateUserOutput CreateUserUseCase::execute(const CreateUserInput& input) {
    // criar entidade
    User user(
        generate_id(),
        input.name,
        input.email,
        input.password
    );

    // salvar
    userRepository->save(user);

    return { user.get_id() };
}