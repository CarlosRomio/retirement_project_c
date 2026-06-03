#include "app-context.hpp"

AppContext::AppContext(const std::string& conn_str)
    : db(conn_str),
    userRepository(db),
    createUserUseCase(userRepository)
{}