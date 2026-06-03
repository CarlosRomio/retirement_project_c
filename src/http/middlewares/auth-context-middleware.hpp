#pragma once
#include <drogon/HttpMiddleware.h>
#include "core/auth/auth-permission.hpp"

namespace drogon {

// Middleware que carrega o contexto de autenticação
class AuthContextMiddleware : public HttpMiddleware<AuthContextMiddleware> {
public:
	void invoke(
		const HttpRequestPtr& req,
		MiddlewareNextCallback&& nextCb,
		MiddlewareCallback&& mcb
	) override;
};

}
