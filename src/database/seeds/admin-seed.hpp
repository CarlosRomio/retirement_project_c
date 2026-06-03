#pragma once
#include <string>
#include <vector>

namespace seeds {
	struct AdminSeed {
		std::string id = "admin-seed-001";
		std::string email = "admin@reiment.local";
		std::string name = "Administrator";
	};

	// Dados do admin seed
	inline const AdminSeed ADMIN_SEED_DATA;
}
