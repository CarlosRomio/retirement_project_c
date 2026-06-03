#include "trip-controller.hpp"
#include <stdexcept>

#include "application/use-cases/create-trip/create-trip-dto.hpp"
#include "core/app-context-holder.hpp"
#include "core/auth/auth-helpers.hpp"
#include "http/controllers/utils/http-helper.hpp"

TripController::TripController()
	: context(AppContextHolder::instance()) {}

void TripController::create(
	const drogon::HttpRequestPtr& req,
	std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
	try {
		// Requer autenticação e permissão para criar viagem
		AUTH_REQUIRE_AUTHENTICATED(req, callback);
		AUTH_REQUIRE_PERMISSION(authCtx, auth::Permission::CREATE_TRIP, callback);

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

		// O owner_user_id é sempre o usuário autenticado
		CreateTripInput input;
		input.name = name;
		input.owner_user_id = authCtx->user_id;

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
	try {
		// Requer autenticação
		AUTH_REQUIRE_AUTHENTICATED(req, callback);
		AUTH_REQUIRE_PERMISSION(authCtx, auth::Permission::READ_TRIP, callback);

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
	try {
		// Requer autenticação e permissão de delete
		AUTH_REQUIRE_AUTHENTICATED(req, callback);
		AUTH_REQUIRE_PERMISSION(authCtx, auth::Permission::DELETE_TRIP, callback);

		// Verifica se é owner da trip ou admin
		auto tripOpt = context.getTripUseCase.execute(id);
		if (!tripOpt) {
			callback(responses::notFound("Trip not found"));
			return;
		}

		AUTH_REQUIRE_OWNERSHIP(authCtx, tripOpt->owner_user_id, callback);

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
