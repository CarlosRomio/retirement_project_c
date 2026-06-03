#pragma once
#include <optional>
#include <string>

#include "get-trip-dto.hpp"

class ITripRepository;

class GetTripUseCase {
private:
	ITripRepository* tripRepository;

public:
	explicit GetTripUseCase(ITripRepository& repo);

	std::optional<GetTripOutput> execute(const std::string& id);
};
