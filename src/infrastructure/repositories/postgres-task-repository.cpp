#include "postgres-task-repository.hpp"

#include <pqxx/pqxx>
#include <stdexcept>

PostgresTaskRepository::PostgresTaskRepository(DatabaseConnection& db)
	: db(db) {}

Task PostgresTaskRepository::save(const Task& task) {
	pqxx::work txn(db.get());

	try {
		auto ownerInTripResult = txn.exec_params(
			"SELECT 1 FROM trip_users WHERE trip_id = $1 AND user_id = $2 LIMIT 1",
			task.get_trip_id(),
			task.get_owner_user_id()
		);

		if (ownerInTripResult.empty()) {
			throw std::runtime_error("Task owner must be a participant of the trip");
		}

		txn.exec_params(
			"INSERT INTO tasks (id, trip_id, owner_user_id, name, description, credits, completed) VALUES ($1, $2, $3, $4, $5, $6, $7)",
			task.get_id(),
			task.get_trip_id(),
			task.get_owner_user_id(),
			task.get_name(),
			task.get_description(),
			task.get_credits(),
			task.is_completed()
		);

		for (const auto& userId : task.get_completed_user_ids()) {
			txn.exec_params(
				"INSERT INTO task_completed_users (task_id, user_id) VALUES ($1, $2)",
				task.get_id(),
				userId
			);
		}

		txn.commit();

		return task;
	}
	catch (const pqxx::unique_violation&) {
		throw std::runtime_error("Task with this id already exists");
	}
	catch (const pqxx::foreign_key_violation&) {
		throw std::runtime_error("Invalid trip_id, owner_user_id or completed_user_ids");
	}
	catch (const pqxx::sql_error& e) {
		throw std::runtime_error(std::string("Database error: ") + e.what());
	}
}

std::optional<Task> PostgresTaskRepository::findById(const std::string& id) {
	try {
		pqxx::work txn(db.get());

		auto taskResult = txn.exec_params(
			"SELECT id, trip_id, owner_user_id, name, description, credits, completed FROM tasks WHERE id = $1",
			id
		);

		if (taskResult.empty()) {
			return std::nullopt;
		}

		auto completedUsersResult = txn.exec_params(
			"SELECT user_id FROM task_completed_users WHERE task_id = $1 ORDER BY user_id",
			id
		);

		std::vector<std::string> completedUserIds;
		completedUserIds.reserve(completedUsersResult.size());

		for (const auto& row : completedUsersResult) {
			completedUserIds.emplace_back(row["user_id"].c_str());
		}

		const auto& row = taskResult[0];

		return Task(
			row["id"].c_str(),
			row["trip_id"].c_str(),
			row["owner_user_id"].c_str(),
			row["name"].is_null() ? "" : row["name"].c_str(),
			row["description"].is_null() ? "" : row["description"].c_str(),
			row["credits"].is_null() ? 0 : row["credits"].as<int>(),
			row["completed"].is_null() ? false : row["completed"].as<bool>(),
			std::move(completedUserIds)
		);
	}
	catch (const pqxx::sql_error& e) {
		throw std::runtime_error(std::string("Database error: ") + e.what());
	}
}

bool PostgresTaskRepository::removeById(const std::string& id) {
	pqxx::work txn(db.get());

	auto res = txn.exec_params(
		"DELETE FROM tasks WHERE id = $1",
		id
	);

	txn.commit();

	return res.affected_rows() > 0;
}

bool PostgresTaskRepository::isOwner(const std::string& task_id, const std::string& user_id) {
	try {
		pqxx::work txn(db.get());

		auto result = txn.exec_params(
			"SELECT 1 FROM tasks WHERE id = $1 AND owner_user_id = $2",
			task_id,
			user_id
		);

		return !result.empty();
	}
	catch (const pqxx::sql_error& e) {
		throw std::runtime_error(std::string("Database error: ") + e.what());
	}
}
