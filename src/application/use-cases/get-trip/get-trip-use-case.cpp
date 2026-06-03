#include "get-trip-use-case.hpp"

#include "domain/repositories/trip-repository.hpp"

GetTripUseCase::GetTripUseCase(ITripRepository& repo)
	: tripRepository(&repo) {}

std::optional<GetTripOutput> GetTripUseCase::execute(const std::string& id) {
	auto tripOpt = tripRepository->findById(id);

	if (!tripOpt) {
		return std::nullopt;
	}

	const auto& trip = *tripOpt;

	return GetTripOutput{
		trip.get_id(),
		trip.get_name(),
		trip.get_created_at(),
		trip.get_task_id(),
		trip.get_user_ids()
	};
}
