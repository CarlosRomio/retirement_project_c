#pragma once
#include "auth-permission.hpp"
#include <drogon/HttpController.h>
#include <drogon/HttpRequest.h>
#include <functional>
#include <optional>

namespace auth {
	// Resultado de validação
	struct ValidationResult {
		bool valid;
		std::string error_message;
		std::optional<AuthContext> context;

		static ValidationResult success(const AuthContext& ctx) {
			return { true, "", ctx };
		}

		static ValidationResult failure(const std::string& msg) {
			return { false, msg, std::nullopt };
		}
	};

	// Helper para extrair e validar AuthContext da request
	class AuthValidator {
	public:
		// Extrai user_id do header
		static std::optional<std::string> extractUserId(const drogon::HttpRequestPtr& req) {
			const auto& user_id = req->getHeader("x-user-id");
			if (user_id.empty()) {
				return std::nullopt;
			}
			return user_id;
		}

		// Valida que user está autenticado (apenas verifica header)
		// O contexto completo será carregado do banco em middleware
		static ValidationResult requireAuthentication(const drogon::HttpRequestPtr& req) {
			auto user_id = extractUserId(req);
			if (!user_id.has_value()) {
				return ValidationResult::failure("Missing x-user-id header");
			}
			return ValidationResult::success(AuthContext(*user_id, "", "", Role::USER));
		}

		// Valida que user tem uma permissão específica
		static ValidationResult requirePermission(
			const AuthContext& ctx,
			Permission permission
		) {
			if (!ctx.hasPermission(permission)) {
				return ValidationResult::failure(
					"Insufficient permissions. Required: " + std::to_string(static_cast<int>(permission))
				);
			}
			return ValidationResult::success(ctx);
		}

		// Valida que user é owner de um recurso
		static ValidationResult requireOwnership(
			const AuthContext& ctx,
			const std::string& resource_owner_id
		) {
			if (ctx.user_id != resource_owner_id && !ctx.isAdmin()) {
				return ValidationResult::failure("You don't have permission to modify this resource");
			}
			return ValidationResult::success(ctx);
		}

		// Valida que user é admin
		static ValidationResult requireAdmin(const AuthContext& ctx) {
			if (!ctx.isAdmin()) {
				return ValidationResult::failure("Admin access required");
			}
			return ValidationResult::success(ctx);
		}

		// Valida múltiplas permissões (todas necessárias)
		static ValidationResult requireAllPermissions(
			const AuthContext& ctx,
			const std::vector<Permission>& permissions
		) {
			if (!ctx.hasAllPermissions(permissions)) {
				return ValidationResult::failure("Insufficient permissions");
			}
			return ValidationResult::success(ctx);
		}

		// Valida uma de múltiplas permissões
		static ValidationResult requireAnyPermission(
			const AuthContext& ctx,
			const std::vector<Permission>& permissions
		) {
			if (!ctx.hasAnyPermission(permissions)) {
				return ValidationResult::failure("Insufficient permissions");
			}
			return ValidationResult::success(ctx);
		}
	};

	// Tipos para callbacks
	using AuthCallback = std::function<void(const drogon::HttpResponsePtr&)>;
	using AuthSuccessHandler = std::function<void(const AuthContext&, const AuthCallback&)>;

	// Helper para criar resposta de erro de autorização
	inline drogon::HttpResponsePtr createForbiddenResponse(const std::string& message) {
		Json::Value body;
		body["error"] = message;
		auto response = drogon::HttpResponse::newHttpJsonResponse(body);
		response->setStatusCode(drogon::k403Forbidden);
		return response;
	}

	inline drogon::HttpResponsePtr createUnauthorizedResponse(const std::string& message) {
		Json::Value body;
		body["error"] = message;
		auto response = drogon::HttpResponse::newHttpJsonResponse(body);
		response->setStatusCode(drogon::k401Unauthorized);
		return response;
	}
}
