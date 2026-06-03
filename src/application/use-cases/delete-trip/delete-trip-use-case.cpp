#include "delete-trip-use-case.hpp"

#include "domain/repositories/trip-repository.hpp"

DeleteTripUseCase::DeleteTripUseCase(ITripRepository& repo)
	: tripRepository(&repo) {}

bool DeleteTripUseCase::execute(const std::string& id) {
	return tripRepository->removeById(id);
}
