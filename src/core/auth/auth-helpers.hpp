#pragma once
#include "auth-context-loader.hpp"
#include "core/app-context-holder.hpp"
#include <drogon/HttpResponse.h>

namespace auth {
	// Helper para usar nos controllers
	inline std::optional<AuthContext> loadAuthContextFromRequest(
		const drogon::HttpRequestPtr& req
	) {
		try {
			auto& context = AppContextHolder::instance();
			return AuthContextLoader::loadFullContext(
				req,
				[&context](const std::string& userId) {
					return context.userRepository.findById(userId);
				}
			);
		}
		catch (const std::exception&) {
			return std::nullopt;
		}
	}
}

// Macro simplificado para usar nos controllers
#define AUTH_REQUIRE_AUTHENTICATED(req, callback) \
	auto authCtx = auth::loadAuthContextFromRequest(req); \
	if (!authCtx) { \
		callback(auth::createUnauthorizedResponse("Authentication failed")); \
		return; \
	}

#define AUTH_REQUIRE_PERMISSION(authCtx, perm, callback) \
	{ \
		auto validation = auth::AuthValidator::requirePermission(*authCtx, perm); \
		if (!validation.valid) { \
			callback(auth::createForbiddenResponse(validation.error_message)); \
			return; \
		} \
	}

#define AUTH_REQUIRE_OWNERSHIP(authCtx, owner_id, callback) \
	{ \
		auto validation = auth::AuthValidator::requireOwnership(*authCtx, owner_id); \
		if (!validation.valid) { \
			callback(auth::createForbiddenResponse(validation.error_message)); \
			return; \
		} \
	}

#define AUTH_REQUIRE_ADMIN(authCtx, callback) \
	{ \
		auto validation = auth::AuthValidator::requireAdmin(*authCtx); \
		if (!validation.valid) { \
			callback(auth::createForbiddenResponse(validation.error_message)); \
			return; \
		} \
	}
