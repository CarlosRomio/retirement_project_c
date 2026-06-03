#pragma once
#include <string>

class ITaskRepository;

class DeleteTaskUseCase {
private:
	ITaskRepository* taskRepository;

public:
	explicit DeleteTaskUseCase(ITaskRepository& repo);

	bool execute(const std::string& id);
};
