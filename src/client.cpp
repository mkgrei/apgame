#include <apgame/core/context.hpp>

#include <apgame/core/client.hpp>
#include <apgame/core/client_option.hpp>
#include <apgame/game/game_client.hpp>

#include <iostream>
#include <string>

int main (int argc, char ** argv) {
  apgame::client_option opt;

  opt.remote_port(12345);
  opt.remote_address("127.0.0.1");

  apgame::client client(opt);

  client.run(apgame::game_client(apgame::REVERSI));
}
