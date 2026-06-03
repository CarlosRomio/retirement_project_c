#pragma once
#include "create-trip-dto.hpp"

class ITripRepository;

class CreateTripUseCase {
private:
	ITripRepository* tripRepository;

public:
	explicit CreateTripUseCase(ITripRepository& repo);

	CreateTripOutput execute(const CreateTripInput& input);
};
