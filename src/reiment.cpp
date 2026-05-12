#include <iostream>
#include "core/services/web-server-service.hpp"

int main(int argc, char* argv[]) {
    WebServerService server;

    server.start_process();

    std::cout << "Servidor rodando em http://localhost:8080\n";
    std::cout << "Pressione ENTER para encerrar...\n";

    std::cin.get(); // mantém o processo vivo

    server.stop_process();

    return 0;
}