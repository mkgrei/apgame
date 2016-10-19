#include <apgame/socket/SocketClient.hpp>
#include <apgame/socket/SocketContext.hpp>

int main () {

  using namespace apgame;

  SocketClientOption opt;

  SocketClient client(opt);
  client.run([&] (SocketContext & ctx) {
    std::cout << "hello" << std::endl;
  });
}
