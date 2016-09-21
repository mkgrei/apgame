#include <apgame/core/context.hpp>

#include <apgame/game/reversi_player.hpp>

#include <array>
#include <chrono>
#include <iostream>
#include <string>
#include <random>

void myplayer (bool is_black, std::array<apgame::reversi_stone, 64> const & board, int & x, int & y) {
  static int X = 0;
  static int Y = 0;

  ++X;
  if (X == 8) {
    ++Y;
    X = 0;
  }
  if (Y == 8) {
    X = 0;
    Y = 0;
  }
  x = X;
  y = Y;
}

int main (int argc, char ** argv) {
  apgame::client_option opt;

  opt.remote_port(12345);
  opt.remote_address("127.0.0.1");

  apgame::reversi_player player(opt, "mygame");
  player.run(myplayer);
}
