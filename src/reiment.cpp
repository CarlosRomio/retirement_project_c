#include <iostream>

#include "bootstrap/web-server-service.hpp"
#include "core/app-context.hpp"
#include "core/app-context-holder.hpp"
#include "core/config/app-config.hpp"
#include "database/migration-runner.hpp"

int main(int argc, char* argv[]) {
	std::cout << "Iniciando aplicação...\n";

	std::cout << "Conectando ao banco de dados...\n";
	AppContext context(config::getDatabaseConnectionString());
	AppContextHolder::setInstance(context);

	std::cout << "Rodando migrations...\n";
	std::string path = MIGRATIONS_PATH;

	if (!std::filesystem::exists(path)) {
		path = "./migrations";
	}

	MigrationRunner runner(path);
	runner.run(context.db.get());

	std::cout << "Inicializando dados de seed (admin)...\n";
	context.initializeAdminUseCase.ensureAdminExists();

	std::cout << "Iniciando servidor web...\n";
	WebServerService server(context);

	server.start();

	return 0;
}