#include "trip-controller.hpp"
#include <stdexcept>

#include "application/use-cases/create-trip/create-trip-dto.hpp"
#include "core/app-context-holder.hpp"
#include "http/controllers/utils/http-helper.hpp"

TripController::TripController()
	: context(AppContextHolder::instance()) {}

void TripController::create(
	const drogon::HttpRequestPtr& req,
	std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
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

		const auto name = (*json)["name"].asString();

		if (name.empty()) {
			callback(responses::badRequest("Name cannot be empty"));
			return;
		}

		CreateTripInput input;
		input.name = name;

		if (json->isMember("task_id") && !(*json)["task_id"].isNull()) {
			input.task_id = (*json)["task_id"].asString();
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

		if (trip.task_id.has_value()) {
			res["task_id"] = *trip.task_id;
		}
		else {
			res["task_id"] = Json::nullValue;
		}

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
