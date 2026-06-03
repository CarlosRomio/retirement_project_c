#pragma once
#include <string>
#include <vector>

namespace auth {
	// Roles disponíveis no sistema
	enum class Role {
		USER,   // Usuário comum
		OWNER,  // Proprietário de viagens (pode gerenciar suas viagens)
		ADMIN   // Administrador (acesso total)
	};

	// Permissões granulares
	enum class Permission {
		// User management
		CREATE_USER,
		UPDATE_OWN_USER,
		DELETE_OWN_USER,
		READ_USER,

		// Trip management
		CREATE_TRIP,
		READ_TRIP,
		UPDATE_TRIP,
		DELETE_TRIP,
		ADD_TRIP_PARTICIPANT,
		REMOVE_TRIP_PARTICIPANT,

		// Task management
		CREATE_TASK,
		READ_TASK,
		UPDATE_TASK,
		DELETE_TASK,
		MARK_TASK_COMPLETE,

		// Admin operations
		MANAGE_USERS,
		MANAGE_ROLES,
		VIEW_LOGS,
		ADMIN_ACCESS
	};

	// Struct para contexto de autenticação com permissões
	struct AuthContext {
		std::string user_id;
		std::string email;
		std::string name;
		Role role = Role::USER;
		std::vector<Permission> permissions;

		// Construtor
		AuthContext() = default;
		AuthContext(
			const std::string& uid,
			const std::string& em,
			const std::string& nm,
			Role r = Role::USER
		) : user_id(uid), email(em), name(nm), role(r) {}

		// Verifica se user tem permissão
		bool hasPermission(Permission perm) const {
			for (const auto& p : permissions) {
				if (p == perm) return true;
			}
			return false;
		}

		// Verifica se user tem uma das permissões
		bool hasAnyPermission(const std::vector<Permission>& perms) const {
			for (const auto& p : perms) {
				if (hasPermission(p)) return true;
			}
			return false;
		}

		// Verifica se user tem todas as permissões
		bool hasAllPermissions(const std::vector<Permission>& perms) const {
			for (const auto& p : perms) {
				if (!hasPermission(p)) return false;
			}
			return true;
		}

		bool isAdmin() const { return role == Role::ADMIN; }
		bool isOwner() const { return role == Role::OWNER || role == Role::ADMIN; }
	};

	// Helper para converter Role to string
	inline std::string roleToString(Role role) {
		switch (role) {
			case Role::USER: return "USER";
			case Role::OWNER: return "OWNER";
			case Role::ADMIN: return "ADMIN";
			default: return "UNKNOWN";
		}
	}

	// Helper para converter string para Role
	inline Role stringToRole(const std::string& str) {
		if (str == "OWNER") return Role::OWNER;
		if (str == "ADMIN") return Role::ADMIN;
		return Role::USER;
	}

	// Helper para obter permissões padrão por role
	inline std::vector<Permission> getDefaultPermissions(Role role) {
		std::vector<Permission> perms;

		// Todas os roles básicas
		perms.push_back(Permission::CREATE_USER);
		perms.push_back(Permission::READ_USER);

		// Permissões por role
		switch (role) {
			case Role::USER:
				perms.push_back(Permission::UPDATE_OWN_USER);
				perms.push_back(Permission::DELETE_OWN_USER);
				perms.push_back(Permission::CREATE_TRIP);
				perms.push_back(Permission::READ_TRIP);
				perms.push_back(Permission::CREATE_TASK);
				perms.push_back(Permission::READ_TASK);
				perms.push_back(Permission::MARK_TASK_COMPLETE);
				break;

			case Role::OWNER:
				perms.push_back(Permission::UPDATE_OWN_USER);
				perms.push_back(Permission::DELETE_OWN_USER);
				perms.push_back(Permission::CREATE_TRIP);
				perms.push_back(Permission::READ_TRIP);
				perms.push_back(Permission::UPDATE_TRIP);
				perms.push_back(Permission::DELETE_TRIP);
				perms.push_back(Permission::ADD_TRIP_PARTICIPANT);
				perms.push_back(Permission::REMOVE_TRIP_PARTICIPANT);
				perms.push_back(Permission::CREATE_TASK);
				perms.push_back(Permission::READ_TASK);
				perms.push_back(Permission::UPDATE_TASK);
				perms.push_back(Permission::DELETE_TASK);
				perms.push_back(Permission::MARK_TASK_COMPLETE);
				break;

			case Role::ADMIN:
				// Admin tem tudo
				perms.push_back(Permission::UPDATE_OWN_USER);
				perms.push_back(Permission::DELETE_OWN_USER);
				perms.push_back(Permission::CREATE_TRIP);
				perms.push_back(Permission::READ_TRIP);
				perms.push_back(Permission::UPDATE_TRIP);
				perms.push_back(Permission::DELETE_TRIP);
				perms.push_back(Permission::ADD_TRIP_PARTICIPANT);
				perms.push_back(Permission::REMOVE_TRIP_PARTICIPANT);
				perms.push_back(Permission::CREATE_TASK);
				perms.push_back(Permission::READ_TASK);
				perms.push_back(Permission::UPDATE_TASK);
				perms.push_back(Permission::DELETE_TASK);
				perms.push_back(Permission::MARK_TASK_COMPLETE);
				perms.push_back(Permission::MANAGE_USERS);
				perms.push_back(Permission::MANAGE_ROLES);
				perms.push_back(Permission::VIEW_LOGS);
				perms.push_back(Permission::ADMIN_ACCESS);
				break;
		}

		return perms;
	}
}
