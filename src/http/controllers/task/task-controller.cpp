#include "task-controller.hpp"
#include <stdexcept>

#include "application/use-cases/create-task/create-task-dto.hpp"
#include "core/app-context-holder.hpp"
#include "core/auth/auth-helpers.hpp"
#include "http/controllers/utils/http-helper.hpp"

TaskController::TaskController()
	: context(AppContextHolder::instance()) {}

void TaskController::create(
	const drogon::HttpRequestPtr& req,
	std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
	try {
		// Requer autenticação e permissão para criar tarefa
		AUTH_REQUIRE_AUTHENTICATED(req, callback);
		AUTH_REQUIRE_PERMISSION(authCtx, auth::Permission::CREATE_TASK, callback);

		auto json = req->getJsonObject();
		if (!json) {
			callback(responses::badRequest("Invalid JSON"));
			return;
		}

		if (!json->isMember("trip_id")) {
			callback(responses::badRequest("Missing trip_id"));
			return;
		}

		if (!json->isMember("name")) {
			callback(responses::badRequest("Missing name"));
			return;
		}

		if (!json->isMember("description")) {
			callback(responses::badRequest("Missing description"));
			return;
		}

		if (!json->isMember("credits")) {
			callback(responses::badRequest("Missing credits"));
			return;
		}

		const auto tripId = (*json)["trip_id"].asString();
		const auto name = (*json)["name"].asString();
		const auto description = (*json)["description"].asString();
		const auto credits = (*json)["credits"].asInt();

		CreateTaskInput input;
		input.trip_id = tripId;
		input.owner_user_id = authCtx->user_id;  // Owner é sempre o usuário autenticado
		input.name = name;
		input.description = description;
		input.credits = credits;
		input.completed = false;

		if (json->isMember("completed")) {
			input.completed = (*json)["completed"].asBool();
		}

		if (json->isMember("completed_user_ids") && (*json)["completed_user_ids"].isArray()) {
			for (const auto& userId : (*json)["completed_user_ids"]) {
				input.completed_user_ids.push_back(userId.asString());
			}
		}

		auto result = context.createTaskUseCase.execute(input);

		Json::Value res;
		res["id"] = result.id;

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

void TaskController::getById(
	const drogon::HttpRequestPtr& req,
	std::function<void(const drogon::HttpResponsePtr&)>&& callback,
	const std::string& id
) {
	try {
		// Requer autenticação
		AUTH_REQUIRE_AUTHENTICATED(req, callback);
		AUTH_REQUIRE_PERMISSION(authCtx, auth::Permission::READ_TASK, callback);

		auto taskOpt = context.getTaskUseCase.execute(id);

		if (!taskOpt) {
			callback(responses::notFound("Task not found"));
			return;
		}

		const auto& task = *taskOpt;

		Json::Value res;
		res["id"] = task.id;
		res["trip_id"] = task.trip_id;
		res["owner_user_id"] = task.owner_user_id;
		res["name"] = task.name;
		res["description"] = task.description;
		res["credits"] = task.credits;
		res["completed"] = task.completed;

		Json::Value completedUsers(Json::arrayValue);
		for (const auto& userId : task.completed_user_ids) {
			completedUsers.append(userId);
		}
		res["completed_user_ids"] = completedUsers;

		callback(drogon::HttpResponse::newHttpJsonResponse(res));
	}
	catch (const std::exception&) {
		callback(responses::internalServerError());
	}
}

void TaskController::remove(
	const drogon::HttpRequestPtr& req,
	std::function<void(const drogon::HttpResponsePtr&)>&& callback,
	const std::string& id
) {
	try {
		// Requer autenticação e permissão de delete
		AUTH_REQUIRE_AUTHENTICATED(req, callback);
		AUTH_REQUIRE_PERMISSION(authCtx, auth::Permission::DELETE_TASK, callback);

		// Verifica se é owner da task ou admin
		auto taskOpt = context.getTaskUseCase.execute(id);
		if (!taskOpt) {
			callback(responses::notFound("Task not found"));
			return;
		}

		AUTH_REQUIRE_OWNERSHIP(authCtx, taskOpt->owner_user_id, callback);

		const auto removed = context.deleteTaskUseCase.execute(id);

		if (!removed) {
			callback(responses::notFound("Task not found"));
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
