#pragma once
#include <drogon/HttpController.h>

#include "core/app-context.hpp"

class TaskController : public drogon::HttpController<TaskController> {
public:
	TaskController();

	METHOD_LIST_BEGIN
		ADD_METHOD_TO(TaskController::create, "/api/tasks", drogon::Post);
		ADD_METHOD_TO(TaskController::getById, "/api/tasks/{1}", drogon::Get);
		ADD_METHOD_TO(TaskController::remove, "/api/tasks/{1}", drogon::Delete);
		ADD_METHOD_TO(TaskController::options, "/api/tasks", drogon::Options);
		ADD_METHOD_TO(TaskController::optionsById, "/api/tasks/{1}", drogon::Options);
	METHOD_LIST_END

	void create(
		const drogon::HttpRequestPtr& req,
		std::function<void(const drogon::HttpResponsePtr&)>&& callback
	);

	void getById(
		const drogon::HttpRequestPtr& req,
		std::function<void(const drogon::HttpResponsePtr&)>&& callback,
		const std::string& id
	);

	void remove(
		const drogon::HttpRequestPtr& req,
		std::function<void(const drogon::HttpResponsePtr&)>&& callback,
		const std::string& id
	);

	void options(
		const drogon::HttpRequestPtr& req,
		std::function<void(const drogon::HttpResponsePtr&)>&& callback
	);

	void optionsById(
		const drogon::HttpRequestPtr& req,
		std::function<void(const drogon::HttpResponsePtr&)>&& callback,
		const std::string& id
	);

private:
	AppContext& context;
};
