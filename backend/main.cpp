#include "crow/middlewares/cors.h"
#include <atomic>
#include <chrono>
#include <crow.h>
#include <crow/json.h>
#include <crow/websocket.h>
#include <format>
#include <mutex>
#include <thread>
#include <unordered_set>

class CustomLogger : public crow::ILogHandler {
  mutable std::mutex mtx;
  std::unordered_set<crow::websocket::connection *> connections;

public:
  CustomLogger() {}
  void log(std::string message, crow::LogLevel /*level*/) {
    std::cerr << ">>> " << message << std::endl;
    std::lock_guard lock{mtx};
    for (auto c : connections) {
      c->send_text(message);
    }
  }
  void add_connection(crow::websocket::connection *c) {
    std::lock_guard lock{mtx};
    connections.insert(c);
  }
  void remove_connection(crow::websocket::connection *c) {
    std::lock_guard lock{mtx};
    connections.erase(c);
  }
};

int main() {
  CustomLogger logger;
  crow::logger::setHandler(&logger);

  crow::App<crow::CORSHandler>
      app; // https://en.wikipedia.org/wiki/Cross-origin_resource_sharing

  CROW_ROUTE(app, "/")
  ([](const crow::request &_, crow::response &res) {
    res.redirect("/index.html");
    res.end();
  });

  CROW_ROUTE(app, "/compute1/<int>")
  ([](int x) {
    CROW_LOG_INFO << "...returning 2 x " << x;
    crow::json::wvalue res({{"result", 2 * x}});
    return res;
  });

  CROW_WEBSOCKET_ROUTE(app, "/logs")
      .onopen([&](crow::websocket::connection &conn) {
        CROW_LOG_INFO << "new websocket connection";
        logger.add_connection(&conn);
      })
      .onclose(
          [&](crow::websocket::connection &conn, const std::string &reason) {
            CROW_LOG_INFO << "websocket connection closed: " << reason;
            logger.remove_connection(&conn);
          })
      .onmessage([&](crow::websocket::connection & /*conn*/,
                     const std::string &data, bool is_binary) {
        CROW_LOG_INFO << "websocket data received (unexpected): "
                      << (is_binary ? "(binary)" : data);
      });

  std::atomic<bool> stop_some_logs = false;
  std::thread some_logs([&stop_some_logs]() {
    size_t i = 0;
    while (!stop_some_logs) {
      CROW_LOG_INFO << "some extra logs from C++ " << i;
      i++;
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(1500ms);
    }
    std::cout << "'some logs' stopped.\n";
  });

  app.port(18080).multithreaded().run();

  stop_some_logs = true;
  some_logs.join();
}
