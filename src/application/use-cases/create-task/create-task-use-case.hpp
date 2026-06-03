#pragma once
#include "create-task-dto.hpp"

class ITaskRepository;

class CreateTaskUseCase {
private:
	ITaskRepository* taskRepository;

public:
	explicit CreateTaskUseCase(ITaskRepository& repo);

	CreateTaskOutput execute(const CreateTaskInput& input);
};
