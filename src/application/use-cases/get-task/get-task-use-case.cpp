#include "get-task-use-case.hpp"

#include "domain/repositories/task-repository.hpp"

GetTaskUseCase::GetTaskUseCase(ITaskRepository& repo)
	: taskRepository(&repo) {}

std::optional<GetTaskOutput> GetTaskUseCase::execute(const std::string& id) {
	auto taskOpt = taskRepository->findById(id);

	if (!taskOpt) {
		return std::nullopt;
	}

	const auto& task = *taskOpt;

	return GetTaskOutput{
		task.get_id(),
		task.get_trip_id(),
		task.get_owner_user_id(),
		task.get_name(),
		task.get_description(),
		task.get_credits(),
		task.is_completed(),
		task.get_completed_user_ids()
	};
}
