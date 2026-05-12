#pragma once
#include <string>
#include <stdexcept>

class User {
private:
    std::string id;
    std::string name;
    std::string email;
    std::string password;

public:
    User(const std::string& id,
        const std::string& name,
        const std::string& email,
        const std::string& password)
        : id(id), name(name), email(email), password(password)
    {
        if (name.empty()) throw std::invalid_argument("Name cannot be empty");
        if (email.empty()) throw std::invalid_argument("Email cannot be empty");
        if (password.empty()) throw std::invalid_argument("Password cannot be empty");
    }

    const std::string& get_id() const { return id; }
    const std::string& get_name() const { return name; }
    const std::string& get_email() const { return email; }
};