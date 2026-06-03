#pragma once
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

class Trip {
private:
	std::string id;
	std::string name;
	std::string created_at;
	std::optional<std::string> task_id;
	std::vector<std::string> user_ids;

public:
	Trip(
		const std::string& id,
		const std::string& name,
		const std::string& created_at,
		std::optional<std::string> task_id,
		std::vector<std::string> user_ids
	)
		: id(id),
		name(name),
		created_at(created_at),
		task_id(std::move(task_id)),
		user_ids(std::move(user_ids)) {
		if (name.empty()) throw std::invalid_argument("Name cannot be empty");
	}

	const std::string& get_id() const { return id; }
	const std::string& get_name() const { return name; }
	const std::string& get_created_at() const { return created_at; }
	const std::optional<std::string>& get_task_id() const { return task_id; }
	const std::vector<std::string>& get_user_ids() const { return user_ids; }
};
