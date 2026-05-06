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


// Função auxiliar para ler o conteúdo de um arquivo
std::string read_file_content(const std::string& filepath) {
  std::ifstream file(filepath, std::ios::in | std::ios::binary);
  if (file) {
    std::string contents;
    file.seekg(0, std::ios::end);
    contents.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(&contents[0], contents.size());
    file.close();
    return contents;
  }
  return "";  // Retorna string vazia se o arquivo não puder ser lido
}

float filter_nan(float value) {
  if (std::isnan(value)) {
    return 0.0f;
  }
  return value;
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

std::string extract_filename(const std::string& body_part) {
  size_t filename_pos = body_part.find("filename=\"");
  if (filename_pos == std::string::npos) return "";

  filename_pos += 10;  
  size_t filename_end = body_part.find("\"", filename_pos);
  if (filename_end == std::string::npos) return "";

  return body_part.substr(filename_pos, filename_end - filename_pos);
}

std::string get_boundary(const std::string& content_type) {
  size_t boundary_pos = content_type.find("boundary=");
  if (boundary_pos == std::string::npos) return "";
  return content_type.substr(boundary_pos + 9);  
}

// Função para processar o corpo multipart manualmente
std::pair<std::string, std::string> extract_file_data(const std::string& body, const std::string& boundary) {
  std::string pattern = "--" + boundary + "\r\n";
  size_t start_pos = body.find(pattern);
  if (start_pos == std::string::npos) return {"", ""};

  // Extrair cabeçalhos da parte
  size_t headers_end = body.find("\r\n\r\n", start_pos);
  if (headers_end == std::string::npos) return {"", ""};
  std::string headers = body.substr(start_pos, headers_end - start_pos);

  // Extrair nome do arquivo
  std::string filename = extract_filename(headers);

  // Extrair conteúdo
  start_pos = headers_end + 4;  // Pula os \r\n\r\n
  size_t end_pos = body.find("\r\n--" + boundary, start_pos);
  if (end_pos == std::string::npos) return {"", ""};

  return {filename, body.substr(start_pos, end_pos - start_pos)};
}

std::string extract_text_field(const std::string& body, const std::string& boundary, const std::string& field_name) {
  std::string pattern = "--" + boundary + "\r\n";
  size_t pos = 0;
  while ((pos = body.find(pattern, pos)) != std::string::npos) {
    size_t headers_end = body.find("\r\n\r\n", pos);
    if (headers_end == std::string::npos) break;
    std::string headers = body.substr(pos, headers_end - pos);
    if (headers.find("name=\"" + field_name + "\"") != std::string::npos &&
        headers.find("filename=\"") == std::string::npos) {
      size_t value_start = headers_end + 4;
      size_t value_end = body.find("\r\n--" + boundary, value_start);
      if (value_end == std::string::npos) break;
      return body.substr(value_start, value_end - value_start);
    }
    pos = headers_end;
  }
  return "";
}

void WebServerService::execute_web_server() {
  // ============================================
  // pegar estado da aplicação
  // ============================================
  CROW_ROUTE(app, "/api/state").methods("GET"_method)([this](const crow::request& req) {
    crow::json::wvalue json_response;

    auto res = crow::response(json_response);
    res.add_header("Access-Control-Allow-Origin", "*");
    res.set_header("Content-Type", "application/json; charset=utf-8");
    return res;
  });

  // Rota para a raiz ("/") que também serve o index.html
  // Isso é importante para garantir que a requisição inicial para "/" seja tratada corretamente.
  CROW_ROUTE(app, "/")
  ([&](crow::response& res) {
    res.set_header("Content-Type", "text/html");
    res.write(read_file_content("./public/index.html"));
    res.end();
  });

  try {
    app.port(8888).multithreaded().run();
  } catch (const std::exception&) {
    std::cerr << "Erro ao iniciar o servidor web. Verifique se a porta 80 está disponível."
              << std::endl;
    return;  // Se falhar, não continua com o servidor
  }
}

void WebServerService::stop_process() {
  app.stop();  // Parar o servidor web

  if (thread_web_server && thread_web_server->joinable()) {
    thread_web_server->join();
    delete thread_web_server;
    thread_web_server = nullptr;
  }
}