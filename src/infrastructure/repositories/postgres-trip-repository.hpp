#pragma once

#include <optional>

#include "database/connection/database-connection.hpp"
#include "domain/repositories/trip-repository.hpp"

class PostgresTripRepository : public ITripRepository {
private:
	DatabaseConnection& db;

public:
	explicit PostgresTripRepository(DatabaseConnection& db);

	Trip save(const Trip& trip) override;
	std::optional<Trip> findById(const std::string& id) override;
	bool removeById(const std::string& id) override;
};
