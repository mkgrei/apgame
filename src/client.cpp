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

  if (argc < 3) {
    std::cout << "usage:" << std::endl;
    std::cout << "apgame-client port address" << std::endl;
    return 1;
  }

  int port = std::atoi(argv[1]);
  std::string address(argv[2]);

  opt.remote_port(port);
  opt.remote_address(address);

  apgame::reversi_player player(opt, "mygame");
  player.run(myplayer);
  return 0;
}
