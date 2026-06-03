#pragma once
#include <string>
#include <vector>

struct GetTripOutput {
	std::string id;
	std::string name;
	std::string created_at;
	std::string owner_user_id;
	std::vector<std::string> task_ids;
	std::vector<std::string> user_ids;
};
