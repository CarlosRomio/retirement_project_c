#pragma once
#include <stdexcept>
#include <string>
#include <vector>

class Trip {
private:
	std::string id;
	std::string name;
	std::string created_at;
	std::string owner_user_id;
	std::vector<std::string> task_ids;
	std::vector<std::string> user_ids;

public:
	Trip(
		const std::string& id,
		const std::string& name,
		const std::string& created_at,
		const std::string& owner_user_id,
		std::vector<std::string> task_ids,
		std::vector<std::string> user_ids
	)
		: id(id),
		name(name),
		created_at(created_at),
		owner_user_id(owner_user_id),
		task_ids(std::move(task_ids)),
		user_ids(std::move(user_ids)) {
		if (name.empty()) throw std::invalid_argument("Name cannot be empty");
		if (owner_user_id.empty()) throw std::invalid_argument("Owner user id cannot be empty");
	}

	const std::string& get_id() const { return id; }
	const std::string& get_name() const { return name; }
	const std::string& get_created_at() const { return created_at; }
	const std::string& get_owner_user_id() const { return owner_user_id; }
	const std::vector<std::string>& get_task_ids() const { return task_ids; }
	const std::vector<std::string>& get_user_ids() const { return user_ids; }
};
