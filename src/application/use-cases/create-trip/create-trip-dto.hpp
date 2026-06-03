#pragma once
#include <string>
#include <vector>

struct CreateTripInput {
	std::string name;
	std::string owner_user_id;
	std::vector<std::string> task_ids;
	std::vector<std::string> user_ids;
};

struct CreateTripOutput {
	std::string id;
	std::string created_at;
};
