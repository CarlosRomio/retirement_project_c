#pragma once
#include <string>
#include <vector>

struct CreateTaskInput {
	std::string trip_id;
	std::string owner_user_id;
	std::string name;
	std::string description;
	int credits;
	bool completed;
	std::vector<std::string> completed_user_ids;
};

struct CreateTaskOutput {
	std::string id;
};
