#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <cctype>
#include <cstdlib>
#include <deque>
#include <filesystem>
#include <fstream>
#include <sstream>
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
  std::string get_logs();
  bool new_backup();
  std::string get_backup(std::string filename);
  void restore_backup(std::string filename);
  std::vector<std::string> get_backups();

  std::thread* thread_web_server = nullptr;
  std::mutex logs_mutex;
};