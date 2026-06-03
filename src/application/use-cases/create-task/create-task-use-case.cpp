#include "create-task-use-case.hpp"

#include <random>

#include "domain/entities/task.hpp"
#include "domain/repositories/task-repository.hpp"

namespace {
	std::string generateTaskId() {
		static thread_local std::mt19937 generator(std::random_device{}());
		static constexpr char hex[] = "0123456789abcdef";
		std::uniform_int_distribution<int> distribution(0, 15);

		std::string id;
		id.reserve(32);

		for (int i = 0; i < 32; ++i) {
			id.push_back(hex[distribution(generator)]);
		}

		return id;
	}
}

CreateTaskUseCase::CreateTaskUseCase(ITaskRepository& repo)
	: taskRepository(&repo) {}

CreateTaskOutput CreateTaskUseCase::execute(const CreateTaskInput& input) {
	Task task(
		generateTaskId(),
		input.trip_id,
		input.owner_user_id,
		input.name,
		input.description,
		input.credits,
		input.completed,
		input.completed_user_ids
	);

	taskRepository->save(task);

	return { task.get_id() };
}
