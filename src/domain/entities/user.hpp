#pragma once
#include <string>
#include <stdexcept>
#include "core/auth/auth-permission.hpp"

class User {
private:
    std::string id;
    std::string name;
    std::string email;
    auth::Role role;

public:
    User(const std::string& id,
        const std::string& name,
        const std::string& email,
        auth::Role r = auth::Role::USER)
        : id(id), name(name), email(email), role(r)
    {
        if (name.empty()) throw std::invalid_argument("Name cannot be empty");
        if (email.empty()) throw std::invalid_argument("Email cannot be empty");
    }

    const std::string& get_id() const { return id; }
    const std::string& get_name() const { return name; }
    const std::string& get_email() const { return email; }
    auth::Role get_role() const { return role; }

    void set_name(const std::string& new_name) {
        if (new_name.empty()) throw std::invalid_argument("Name cannot be empty");
        name = new_name;
    }

    void set_role(auth::Role r) {
        role = r;
    }
};