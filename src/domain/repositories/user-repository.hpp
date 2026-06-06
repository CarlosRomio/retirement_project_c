#pragma once
#include <optional>

#include "domain/entities/user.hpp"

class IUserRepository {
public:
    virtual void save(const User& user) = 0;
    virtual std::optional<User> findById(const std::string& id) = 0;
    virtual std::optional<User> findByEmail(const std::string& email) = 0;
    virtual void update(const User& user) = 0;
    virtual ~IUserRepository() = default;
};
