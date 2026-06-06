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
			"dbname=retirement_db user=carlos password=12345678 host=localhost port=5432"
		);
	}
}
