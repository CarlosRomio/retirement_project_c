#pragma once
#include <optional>
#include <string>

#include "get-task-dto.hpp"

class ITaskRepository;

class GetTaskUseCase {
private:
	ITaskRepository* taskRepository;

public:
	explicit GetTaskUseCase(ITaskRepository& repo);

	std::optional<GetTaskOutput> execute(const std::string& id);
};
