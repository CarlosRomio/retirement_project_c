#pragma once
#include <drogon/HttpController.h>

#include "core/app-context.hpp"

class TripController : public drogon::HttpController<TripController> {
public:
	TripController();

	METHOD_LIST_BEGIN
		ADD_METHOD_TO(TripController::create, "/api/trips", drogon::Post);
		ADD_METHOD_TO(TripController::getById, "/api/trips/{1}", drogon::Get);
		ADD_METHOD_TO(TripController::remove, "/api/trips/{1}", drogon::Delete);
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

private:
	AppContext& context;
};
