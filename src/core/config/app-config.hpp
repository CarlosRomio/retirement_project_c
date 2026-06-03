#pragma once
#include <cstdlib>
#include <string>

namespace config {
	inline std::string getEnvOrDefault(const char* name, const char* defaultValue) {
		const char* value = std::getenv(name);

		if (value == nullptr || std::string(value).empty()) {
			return defaultValue;
		}

		return value;
	}

	inline std::string getDatabaseConnectionString() {
		return getEnvOrDefault(
			"DATABASE_URL",
			"dbname=travelapp user=postgres password=12345678 host=172.20.0.1"
		);
	}
}
