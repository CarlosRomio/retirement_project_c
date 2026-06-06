#include "auth-controller.hpp"
#include <stdexcept>
#include <ctime>
#include <sstream>
#include <iomanip>

#include "core/app-context-holder.hpp"
#include "http/controllers/utils/http-helper.hpp"
#include "http/controllers/utils/logging.hpp"

AuthController::AuthController()
    : context(AppContextHolder::instance()) {}

void AuthController::login(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    logging::logRequest("POST", "/api/auth/login");

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

        const auto email = (*json)["email"].asString();

        if (email.empty()) {
            callback(responses::badRequest("Email cannot be empty"));
            return;
        }

        auto userOpt = context.userRepository.findByEmail(email);
        if (!userOpt) {
            callback(responses::unauthorized("Invalid credentials"));
            return;
        }

        const auto& user = *userOpt;

        Json::Value res;
        res["id"] = user.get_id();
        res["name"] = user.get_name();
        res["email"] = user.get_email();
        res["token"] = user.get_id();

        auto response = drogon::HttpResponse::newHttpJsonResponse(res);
        response->setStatusCode(drogon::k200OK);
        callback(response);
    }
    catch (const std::invalid_argument& e) {
        callback(responses::badRequest(e.what()));
    }
    catch (const std::exception&) {
        callback(responses::internalServerError());
    }
}

void AuthController::options(
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
