#pragma once

#include <optional>

#include "database/connection/database-connection.hpp"
#include "domain/repositories/task-repository.hpp"

class PostgresTaskRepository : public ITaskRepository {
private:
	DatabaseConnection& db;

public:
	explicit PostgresTaskRepository(DatabaseConnection& db);

	Task save(const Task& task) override;
	std::optional<Task> findById(const std::string& id) override;
	bool removeById(const std::string& id) override;
	bool isOwner(const std::string& task_id, const std::string& user_id) override;
};
