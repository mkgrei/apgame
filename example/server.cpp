#include <apgame/socket/SocketContext.hpp>
#include <apgame/socket/SocketServer.hpp>

int main () {

  using namespace apgame;

  SocketServerOption opt;

  SocketServer server(opt);
  server.run([&] (SocketContext & ctx) {
    std::cout << "hello" << std::endl;
  });
}
