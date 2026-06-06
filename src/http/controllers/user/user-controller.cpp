#include "user-controller.hpp"
#include <stdexcept>

#include "core/app-context-holder.hpp"
#include "http/controllers/utils/http-helper.hpp"
#include "http/controllers/utils/logging.hpp"

UserController::UserController()
    : context(AppContextHolder::instance()) {}

void UserController::create(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    logging::logRequest("POST", "/api/users");

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

        if (!json->isMember("email")) {
            callback(responses::badRequest("Missing email"));
            return;
        }

        const auto name = (*json)["name"].asString();
        const auto email = (*json)["email"].asString();

        if (name.empty()) {
            callback(responses::badRequest("Name cannot be empty"));
            return;
        }

        if (email.empty()) {
            callback(responses::badRequest("Email cannot be empty"));
            return;
        }

        CreateUserInput input;
        input.name = name;
        input.email = email;

        auto result = context.createUserUseCase.execute(input);

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
        callback(responses::conflict(e.what()));
    }
    catch (const std::exception&) {
        callback(responses::internalServerError());
    }
}

void UserController::me(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    logging::logRequest("GET", "/api/users/me");

    try {
        auto authUser = auth::requireAuth(req, callback);
        if (!authUser) {
            return;
        }

        auto userOpt = context.userRepository.findById(authUser->id);

        if (!userOpt) {
            callback(responses::notFound("User not found"));
            return;
        }

        const auto& user = *userOpt;

        Json::Value res;
        res["id"] = user.get_id();
        res["name"] = user.get_name();
        res["email"] = user.get_email();

        callback(drogon::HttpResponse::newHttpJsonResponse(res));
    }
    catch (const std::exception&) {
        callback(responses::internalServerError());
    }
}

void UserController::updateMe(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    logging::logRequest("PUT", "/api/users/me");

    try {
        auto authUser = auth::requireAuth(req, callback);
        if (!authUser) {
            return;
        }

        auto json = req->getJsonObject();
        if (!json) {
            callback(responses::badRequest("Invalid JSON"));
            return;
        }

        if (!json->isMember("name")) {
            callback(responses::badRequest("Missing name"));
            return;
        }

        std::string name = (*json)["name"].asString();

        if (name.empty()) {
            callback(responses::badRequest("Name cannot be empty"));
            return;
        }

        auto userOpt = context.userRepository.findById(authUser->id);

        if (!userOpt) {
            callback(responses::notFound("User not found"));
            return;
        }

        auto user = *userOpt;
        user.set_name(name);

        context.userRepository.update(user);

        Json::Value res;
        res["id"] = user.get_id();
        res["name"] = user.get_name();
        res["email"] = user.get_email();

        callback(drogon::HttpResponse::newHttpJsonResponse(res));
    }
    catch (const std::invalid_argument& e) {
        callback(responses::badRequest(e.what()));
    }
    catch (const std::exception&) {
        callback(responses::internalServerError());
    }
}

void UserController::options(
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

