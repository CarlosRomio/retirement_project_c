#include "web-server-service.hpp"

// Função auxiliar para obter o tipo MIME
std::string get_mime_type(const std::string& filepath) {
  if (filepath.ends_with(".html")) return "text/html";
  if (filepath.ends_with(".css")) return "text/css";
  if (filepath.ends_with(".js")) return "application/javascript";
  if (filepath.ends_with(".json")) return "application/json";
  if (filepath.ends_with(".png")) return "image/png";
  if (filepath.ends_with(".jpg") || filepath.ends_with(".jpeg")) return "image/jpeg";
  if (filepath.ends_with(".gif")) return "image/gif";
  if (filepath.ends_with(".svg")) return "image/svg+xml";
  // Adicione mais tipos MIME conforme necessário
  return "application/octet-stream";  // Tipo MIME padrão para arquivos desconhecidos
}

WebServerService::WebServerService()
    : thread_web_server(nullptr) {}

WebServerService::~WebServerService() { stop_process(); }


void WebServerService::start_process() {
  if (thread_web_server && thread_web_server->joinable()) {
    stop_process();
  }
  thread_web_server = new std::thread(&WebServerService::execute_web_server, this);
}


void WebServerService::execute_web_server() {

    // pegar estado da aplicação
    // ============================================
    CROW_ROUTE(app, "/api/state").methods("GET"_method)([this](const crow::request& req) {
        crow::json::wvalue json_response;
        json_response["status"] = "ok";
        json_response["message"] = "API rodando";
        json_response["timestamp"] = std::time(nullptr);

        auto res = crow::response(json_response);
        res.add_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json; charset=utf-8");
        return res;
        });

    try {
        app.port(8080).multithreaded().signal_clear().run();
    }
    catch (const std::exception&) {
        std::cerr << "Erro ao iniciar o servidor web. Verifique se a porta 8080 está disponível."
            << std::endl;
        return;
    }
}

void WebServerService::stop_process() {
  app.stop();

  if (thread_web_server && thread_web_server->joinable()) {
    thread_web_server->join();
    delete thread_web_server;
    thread_web_server = nullptr;
  }
}