#include "auth-controller.hpp"
#include "application/use-cases/login/login-dto.hpp"
#include "core/app-context-holder.hpp"
#include "http/controllers/utils/http-helper.hpp"

AuthController::AuthController()
	: context(AppContextHolder::instance()) {}

void AuthController::login(
	const drogon::HttpRequestPtr& req,
	std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
	try {
		auto json = req->getJsonObject();
		if (!json) {
			callback(responses::badRequest("Invalid JSON"));
			return;
		}

		if (!json->isMember("email")) {
			callback(responses::badRequest("Missing email"));
			return;
		}

		if (!json->isMember("password")) {
			callback(responses::badRequest("Missing password"));
			return;
		}

		const auto email = (*json)["email"].asString();
		const auto password = (*json)["password"].asString();

		if (email.empty()) {
			callback(responses::badRequest("Email cannot be empty"));
			return;
		}

		if (password.empty()) {
			callback(responses::badRequest("Password cannot be empty"));
			return;
		}

		LoginInput input;
		input.email = email;
		input.password = password;

		auto result = context.loginUseCase.execute(input);

		Json::Value res;
		res["user_id"] = result.user_id;
		res["name"] = result.name;
		res["email"] = result.email;
		res["role"] = result.role;

		callback(drogon::HttpResponse::newHttpJsonResponse(res));
	}
	catch (const std::invalid_argument& e) {
		callback(responses::badRequest(e.what()));
	}
	catch (const std::exception&) {
		callback(responses::internalServerError());
	}
}
