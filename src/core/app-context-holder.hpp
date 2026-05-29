#pragma once
#include <stdexcept>

#include "core/app-context.hpp"

class AppContextHolder {
public:
    static void setInstance(AppContext& context) {
        instancePtr() = &context;
    }

    static AppContext& instance() {
        auto* context = instancePtr();

        if (!context) {
            throw std::runtime_error("AppContext not initialized");
        }

        return *context;
    }

private:
    static AppContext*& instancePtr() {
        static AppContext* context = nullptr;
        return context;
    }
};
