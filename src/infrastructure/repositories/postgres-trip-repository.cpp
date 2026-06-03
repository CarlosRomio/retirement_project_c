#include "postgres-trip-repository.hpp"

#include <pqxx/pqxx>
#include <stdexcept>

PostgresTripRepository::PostgresTripRepository(DatabaseConnection& db)
	: db(db) {}

Trip PostgresTripRepository::save(const Trip& trip) {
	pqxx::work txn(db.get());

	try {
		auto tripResult = txn.exec_params(
			"INSERT INTO trips (id, name, owner_user_id) VALUES ($1, $2, $3) RETURNING created_at",
			trip.get_id(),
			trip.get_name(),
			trip.get_owner_user_id()
		);

		for (const auto& userId : trip.get_user_ids()) {
			txn.exec_params(
				"INSERT INTO trip_users (trip_id, user_id) VALUES ($1, $2)",
				trip.get_id(),
				userId
			);
		}

		for (const auto& taskId : trip.get_task_ids()) {
			txn.exec_params(
				"INSERT INTO trip_tasks (trip_id, task_id) VALUES ($1, $2)",
				trip.get_id(),
				taskId
			);
		}

		txn.commit();

		return Trip(
			trip.get_id(),
			trip.get_name(),
			tripResult[0]["created_at"].c_str(),
			trip.get_owner_user_id(),
			trip.get_task_ids(),
			trip.get_user_ids()
		);
	}
	catch (const pqxx::unique_violation&) {
		throw std::runtime_error("Trip with this id already exists");
	}
	catch (const pqxx::foreign_key_violation&) {
		throw std::runtime_error("Invalid owner_user_id, task_ids or user_ids");
	}
	catch (const pqxx::sql_error& e) {
		throw std::runtime_error(std::string("Database error: ") + e.what());
	}
}

std::optional<Trip> PostgresTripRepository::findById(const std::string& id) {
	try {
		pqxx::work txn(db.get());

		auto tripResult = txn.exec_params(
			"SELECT id, name, created_at, owner_user_id FROM trips WHERE id = $1",
			id
		);

		if (tripResult.empty()) {
			return std::nullopt;
		}

		auto usersResult = txn.exec_params(
			"SELECT user_id FROM trip_users WHERE trip_id = $1 ORDER BY user_id",
			id
		);

		std::vector<std::string> userIds;
		userIds.reserve(usersResult.size());

		for (const auto& row : usersResult) {
			userIds.emplace_back(row["user_id"].c_str());
		}

		auto tasksResult = txn.exec_params(
			"SELECT task_id FROM trip_tasks WHERE trip_id = $1 ORDER BY task_id",
			id
		);

		std::vector<std::string> taskIds;
		taskIds.reserve(tasksResult.size());

		for (const auto& row : tasksResult) {
			taskIds.emplace_back(row["task_id"].c_str());
		}

		const auto& row = tripResult[0];

		return Trip(
			row["id"].c_str(),
			row["name"].c_str(),
			row["created_at"].c_str(),
			row["owner_user_id"].is_null() ? "" : row["owner_user_id"].c_str(),
			std::move(taskIds),
			std::move(userIds)
		);
	}
	catch (const pqxx::sql_error& e) {
		throw std::runtime_error(std::string("Database error: ") + e.what());
	}
}

bool PostgresTripRepository::removeById(const std::string& id) {
	pqxx::work txn(db.get());

	auto res = txn.exec_params(
		"DELETE FROM trips WHERE id = $1",
		id
	);

	txn.commit();

	return res.affected_rows() > 0;
}
