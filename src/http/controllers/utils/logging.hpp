#pragma once
#include <iostream>
#include <chrono>
#include <string>
#include <iomanip>

namespace logging {
    inline void logRequest(const std::string& method, const std::string& path) {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::cout << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
                  << "] " << method << " " << path << std::endl;
    }
}
