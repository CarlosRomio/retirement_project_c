#include "delete-task-use-case.hpp"

#include "domain/repositories/task-repository.hpp"

DeleteTaskUseCase::DeleteTaskUseCase(ITaskRepository& repo)
	: taskRepository(&repo) {}

bool DeleteTaskUseCase::execute(const std::string& id) {
	return taskRepository->removeById(id);
}
