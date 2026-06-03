#include "create-user-use-case.hpp"
#include "domain/repositories/user-repository.hpp"
#include <random>
#include <sstream>

namespace {
    std::string generateUserId() {
        static thread_local std::mt19937 generator(std::random_device{}());
        static constexpr char hex[] = "0123456789abcdef";
        std::uniform_int_distribution<int> distribution(0, 15);

        std::string id;
        id.reserve(32);

        for (int i = 0; i < 32; ++i) {
            id.push_back(hex[distribution(generator)]);
        }

        return id;
    }
}

CreateUserUseCase::CreateUserUseCase(IUserRepository& repo)
    : userRepository(&repo) {
}

CreateUserOutput CreateUserUseCase::execute(const CreateUserInput& input) {
    User user(
        generateUserId(),
        input.name,
        input.email
    );

    userRepository->save(user);

    return { user.get_id() };
}
