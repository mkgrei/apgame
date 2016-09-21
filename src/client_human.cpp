#include <apgame/core/context.hpp>

#include <apgame/game/reversi_player.hpp>

#include <array>
#include <chrono>
#include <iostream>
#include <string>
#include <random>

struct myai {

  void operator() (bool is_black, std::array<apgame::reversi_stone, 64> const & board, int & x, int & y) {
    std::string line;
    x = 100;
    y = 100;

    if (!std::getline(std::cin, line)) {
      return false;
    }
    x = std::stoll(line);
    if (!std::getline(std::cin, line)) {
      return false;
    }
    y = std::stoll(line);
  }
};

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
  player.run(myai());
  return 0;
}
