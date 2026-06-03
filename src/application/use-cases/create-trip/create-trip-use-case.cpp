#include "create-trip-use-case.hpp"

#include <random>

#include "domain/entities/trip.hpp"
#include "domain/repositories/trip-repository.hpp"

namespace {
	std::string generateTripId() {
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

CreateTripUseCase::CreateTripUseCase(ITripRepository& repo)
	: tripRepository(&repo) {}

CreateTripOutput CreateTripUseCase::execute(const CreateTripInput& input) {
	Trip trip(
		generateTripId(),
		input.name,
		"",
		input.task_id,
		input.user_ids
	);

	auto persisted = tripRepository->save(trip);

	return {
		persisted.get_id(),
		persisted.get_created_at()
	};
}
