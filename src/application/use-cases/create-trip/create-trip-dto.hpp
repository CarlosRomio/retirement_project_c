#pragma once
#include <optional>
#include <string>
#include <vector>

struct CreateTripInput {
	std::string name;
	std::optional<std::string> task_id;
	std::vector<std::string> user_ids;
};

struct CreateTripOutput {
	std::string id;
	std::string created_at;
};
