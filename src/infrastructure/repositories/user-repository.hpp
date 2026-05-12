#pragma once
#include "domain/entities/user.hpp"

class IUserRepository {
public:
    virtual void save(const User& user) = 0;
    virtual ~IUserRepository() = default;
};