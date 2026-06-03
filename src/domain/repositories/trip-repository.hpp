#pragma once
#include <optional>
#include <string>

#include "domain/entities/trip.hpp"

class ITripRepository {
public:
	virtual Trip save(const Trip& trip) = 0;
	virtual std::optional<Trip> findById(const std::string& id) = 0;
	virtual bool removeById(const std::string& id) = 0;
	virtual bool isOwner(const std::string& trip_id, const std::string& user_id) = 0;
	virtual ~ITripRepository() = default;
};
