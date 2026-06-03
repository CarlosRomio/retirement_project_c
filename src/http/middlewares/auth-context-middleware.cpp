#include "auth-context-middleware.hpp"
#include "core/app-context-holder.hpp"
#include "core/auth/auth-validator.hpp"
#include <json/json.h>

namespace drogon {

void AuthContextMiddleware::invoke(
	const HttpRequestPtr& req,
	MiddlewareNextCallback&& nextCb,
	MiddlewareCallback&& mcb
) {
	// Rotas públicas que não necessitam autenticação
	static const std::vector<std::string> PUBLIC_ROUTES = {
		"/api/auth/login",
		"/api/users",  // POST para criar usuário
		"/health",
		"/readiness"
	};

	const auto& path = req->getPath();
	const auto& method = req->getMethod();

	// Verifica se é rota pública
	bool isPublicRoute = false;
	for (const auto& route : PUBLIC_ROUTES) {
		if (path == route) {
			isPublicRoute = true;
			break;
		}
	}

	// Se for rota pública, passa adiante
	if (isPublicRoute) {
		nextCb([mcb = std::move(mcb)](const HttpResponsePtr& resp) {
			mcb(resp);
		});
		return;
	}

	// Para rotas protegidas, valida autenticação
	auto userIdOpt = auth::AuthValidator::extractUserId(req);
	if (!userIdOpt) {
		Json::Value body;
		body["error"] = "Missing x-user-id header";
		auto response = drogon::HttpResponse::newHttpJsonResponse(body);
		response->setStatusCode(drogon::k401Unauthorized);
		mcb(response);
		return;
	}

	// Continua para o handler
	nextCb([mcb = std::move(mcb)](const HttpResponsePtr& resp) {
		mcb(resp);
	});
}

}
