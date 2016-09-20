#include <apgame/core/context.hpp>

#include <apgame/game/reversi_player.hpp>

#include <array>
#include <chrono>
#include <iostream>
#include <string>

void myplayer (bool is_black, std::array<apgame::reversi_stone, 64> const & board, int & x, int & y) {
  x = 0;
  y = 0;
  LOG_INFO("myplayer\n");
}

int main (int argc, char ** argv) {
  apgame::client_option opt;

  opt.remote_port(12345);
  opt.remote_address("127.0.0.1");

  apgame::reversi_player player(opt, "mygame");
  player.run(myplayer);
}
