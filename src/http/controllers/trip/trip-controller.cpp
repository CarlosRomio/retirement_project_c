#include "trip-controller.hpp"
#include <stdexcept>

#include "application/use-cases/create-trip/create-trip-dto.hpp"
#include "core/app-context-holder.hpp"
#include "http/controllers/utils/http-helper.hpp"
#include "http/controllers/utils/logging.hpp"

TripController::TripController()
	: context(AppContextHolder::instance()) {}

void TripController::create(
	const drogon::HttpRequestPtr& req,
	std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
	logging::logRequest("POST", "/api/trips");

	try {
		auto json = req->getJsonObject();
		if (!json) {
			callback(responses::badRequest("Invalid JSON"));
			return;
		}

		if (!json->isMember("name")) {
			callback(responses::badRequest("Missing name"));
			return;
		}

		if (!json->isMember("owner_user_id")) {
			callback(responses::badRequest("Missing owner_user_id"));
			return;
		}

		const auto name = (*json)["name"].asString();
		const auto ownerUserId = (*json)["owner_user_id"].asString();

		if (name.empty()) {
			callback(responses::badRequest("Name cannot be empty"));
			return;
		}

		if (ownerUserId.empty()) {
			callback(responses::badRequest("Owner user id cannot be empty"));
			return;
		}

		CreateTripInput input;
		input.name = name;
		input.owner_user_id = ownerUserId;

		if (json->isMember("task_ids") && (*json)["task_ids"].isArray()) {
			for (const auto& taskId : (*json)["task_ids"]) {
				input.task_ids.push_back(taskId.asString());
			}
		}

		if (json->isMember("user_ids") && (*json)["user_ids"].isArray()) {
			for (const auto& userId : (*json)["user_ids"]) {
				input.user_ids.push_back(userId.asString());
			}
		}

		auto result = context.createTripUseCase.execute(input);

		Json::Value res;
		res["id"] = result.id;
		res["created_at"] = result.created_at;

		auto response = drogon::HttpResponse::newHttpJsonResponse(res);
		response->setStatusCode(drogon::k201Created);
		callback(response);
	}
	catch (const std::invalid_argument& e) {
		callback(responses::badRequest(e.what()));
	}
	catch (const std::runtime_error& e) {
		callback(responses::badRequest(e.what()));
	}
	catch (const std::exception&) {
		callback(responses::internalServerError());
	}
}

void TripController::getById(
	const drogon::HttpRequestPtr& req,
	std::function<void(const drogon::HttpResponsePtr&)>&& callback,
	const std::string& id
) {
	logging::logRequest("GET", "/api/trips/" + id);

	try {
		auto tripOpt = context.getTripUseCase.execute(id);

		if (!tripOpt) {
			callback(responses::notFound("Trip not found"));
			return;
		}

		const auto& trip = *tripOpt;

		Json::Value res;
		res["id"] = trip.id;
		res["name"] = trip.name;
		res["created_at"] = trip.created_at;
		res["owner_user_id"] = trip.owner_user_id;

		Json::Value tasks(Json::arrayValue);
		for (const auto& taskId : trip.task_ids) {
			tasks.append(taskId);
		}
		res["task_ids"] = tasks;

		Json::Value users(Json::arrayValue);
		for (const auto& userId : trip.user_ids) {
			users.append(userId);
		}
		res["user_ids"] = users;

		callback(drogon::HttpResponse::newHttpJsonResponse(res));
	}
	catch (const std::exception&) {
		callback(responses::internalServerError());
	}
}

void TripController::remove(
	const drogon::HttpRequestPtr& req,
	std::function<void(const drogon::HttpResponsePtr&)>&& callback,
	const std::string& id
) {
	logging::logRequest("DELETE", "/api/trips/" + id);

	try {
		const auto removed = context.deleteTripUseCase.execute(id);

		if (!removed) {
			callback(responses::notFound("Trip not found"));
			return;
		}

		auto response = drogon::HttpResponse::newHttpResponse();
		response->setStatusCode(drogon::k204NoContent);
		callback(response);
	}
	catch (const std::exception&) {
		callback(responses::internalServerError());
	}
}

void TripController::options(
	const drogon::HttpRequestPtr& req,
	std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
	auto resp = drogon::HttpResponse::newHttpResponse();
	resp->setStatusCode(drogon::k200OK);
	resp->addHeader("Access-Control-Allow-Origin", "*");
	resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS, PATCH");
	resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization, x-user-id");
	resp->addHeader("Access-Control-Max-Age", "86400");
	callback(resp);
}

void TripController::optionsById(
	const drogon::HttpRequestPtr& req,
	std::function<void(const drogon::HttpResponsePtr&)>&& callback,
	const std::string& id
) {
	auto resp = drogon::HttpResponse::newHttpResponse();
	resp->setStatusCode(drogon::k200OK);
	resp->addHeader("Access-Control-Allow-Origin", "*");
	resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS, PATCH");
	resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization, x-user-id");
	resp->addHeader("Access-Control-Max-Age", "86400");
	callback(resp);
}
