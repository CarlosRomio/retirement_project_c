#pragma once
#include "auth-validator.hpp"
#include <drogon/HttpResponse.h>
#include <optional>
#include <functional>

namespace auth {
	// Helper para carregar contexto completo do banco
	class AuthContextLoader {
	public:
		// Carrega contexto completo do usuário (com permissões)
		// Usa callback para aceitar um repositório injetado
		static std::optional<AuthContext> loadFullContext(
			const drogon::HttpRequestPtr& req,
			std::function<std::optional<class User>(const std::string&)> userFinder
		) {
			auto userIdOpt = AuthValidator::extractUserId(req);
			if (!userIdOpt) {
				return std::nullopt;
			}

			try {
				auto userOpt = userFinder(*userIdOpt);

				if (!userOpt) {
					return std::nullopt;
				}

				const auto& user = *userOpt;
				AuthContext authCtx(
					user.get_id(),
					user.get_email(),
					user.get_name(),
					user.get_role()
				);

				// Carrega permissões padrão para o role
				authCtx.permissions = getDefaultPermissions(user.get_role());

				return authCtx;
			}
			catch (const std::exception&) {
				return std::nullopt;
			}
		}
	};

	// Decorador para validação de permissão
	template<Permission RequiredPermission>
	class RequirePermissionDecorator {
	public:
		static bool validate(
			const AuthContext& ctx,
			std::function<void(const drogon::HttpResponsePtr&)>& callback
		) {
			auto validation = AuthValidator::requirePermission(ctx, RequiredPermission);
			if (!validation.valid) {
				callback(createForbiddenResponse(validation.error_message));
				return false;
			}
			return true;
		}
	};
}
