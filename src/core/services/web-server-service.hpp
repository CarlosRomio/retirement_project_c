#pragma once

#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <string>
#include <utility>
#include <vector>
#include <crow.h>
#include <crow/utility.h>

// #include "time_utils.h"

class WebServerService {
 public:
  WebServerService();
  ~WebServerService();

  // ------------------------------------------------------------
  // Métodos de backup, restore, etc.
  // ------------------------------------------------------------
  void start_process();
  void stop_process();

 private:
  void execute_web_server();
  crow::SimpleApp app;

  std::thread* thread_web_server = nullptr;
};