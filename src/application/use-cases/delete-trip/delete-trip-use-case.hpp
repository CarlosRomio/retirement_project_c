#pragma once
#include <string>

class ITripRepository;

class DeleteTripUseCase {
private:
	ITripRepository* tripRepository;

public:
	explicit DeleteTripUseCase(ITripRepository& repo);

	bool execute(const std::string& id);
};
