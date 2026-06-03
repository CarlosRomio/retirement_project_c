#pragma once
#include <optional>
#include <string>

#include "domain/entities/task.hpp"

class ITaskRepository {
public:
	virtual Task save(const Task& task) = 0;
	virtual std::optional<Task> findById(const std::string& id) = 0;
	virtual bool removeById(const std::string& id) = 0;
	virtual ~ITaskRepository() = default;
};
