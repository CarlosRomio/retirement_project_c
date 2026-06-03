#pragma once
#include <stdexcept>
#include <string>
#include <vector>

class Task {
private:
	std::string id;
	std::string trip_id;
	std::string owner_user_id;
	std::string name;
	std::string description;
	int credits;
	bool completed;
	std::vector<std::string> completed_user_ids;

public:
	Task(
		const std::string& id,
		const std::string& trip_id,
		const std::string& owner_user_id,
		const std::string& name,
		const std::string& description,
		int credits,
		bool completed,
		std::vector<std::string> completed_user_ids
	)
		: id(id),
		  trip_id(trip_id),
		  owner_user_id(owner_user_id),
		  name(name),
		  description(description),
		  credits(credits),
		  completed(completed),
		  completed_user_ids(std::move(completed_user_ids)) {
		if (trip_id.empty()) throw std::invalid_argument("Trip id cannot be empty");
		if (owner_user_id.empty()) throw std::invalid_argument("Owner user id cannot be empty");
		if (name.empty()) throw std::invalid_argument("Name cannot be empty");
		if (credits < 0) throw std::invalid_argument("Credits cannot be negative");
	}

	const std::string& get_id() const { return id; }
	const std::string& get_trip_id() const { return trip_id; }
	const std::string& get_owner_user_id() const { return owner_user_id; }
	const std::string& get_name() const { return name; }
	const std::string& get_description() const { return description; }
	int get_credits() const { return credits; }
	bool is_completed() const { return completed; }
	const std::vector<std::string>& get_completed_user_ids() const { return completed_user_ids; }
};
