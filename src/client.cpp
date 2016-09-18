#include <apgame/core/context.hpp>

#include <apgame/game/reversi_client.hpp>

#include <iostream>
#include <string>

void my_game (apgame::reversi_client & client) {
  std::cout << "hello" << std::endl;
  apgame::reversi_client::game_status status;
  client.get_game_status(status);
  std::cout << status << std::endl;
}

int main (int argc, char ** argv) {
  apgame::client_option opt;

  opt.remote_port(12345);
  opt.remote_address("127.0.0.1");

  apgame::reversi_client client(opt);
  client.run(my_game);
}
