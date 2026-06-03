#pragma once
#include <optional>
#include <string>
#include <vector>

struct GetTripOutput {
	std::string id;
	std::string name;
	std::string created_at;
	std::optional<std::string> task_id;
	std::vector<std::string> user_ids;
};
