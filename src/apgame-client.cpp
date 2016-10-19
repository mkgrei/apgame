#include <apgame/socket/SocketClient.hpp>
#include <apgame/socket/SocketContext.hpp>
#include <apgame/reversi/ReversiPlayer.hpp>

#include <boost/program_options.hpp>

#include <array>
#include <chrono>
#include <iostream>
#include <string>
#include <random>

struct myai {

  std::mt19937 random_engine;
  apgame::ReversiPlayer & player;
  myai (apgame::ReversiPlayer & player)
  : player(player) {
    std::random_device random_device;
    random_engine.seed(random_device());
  }

  void operator() (int color, std::array<apgame::ReversiStone, 64> & board, int & x, int & y) {

    if (!player.checkPossibleChoice(color)) {
      x = -1;
      return;
    }
    while (true) {
      x = std::uniform_int_distribution<int>(0, 7)(random_engine);
      y = std::uniform_int_distribution<int>(0, 7)(random_engine);
      if (player.checkPutStone(color, x, y, false)) {
        break;
      }
    }
  }

};

int main (int argc, char ** argv) {

  using namespace boost::program_options;
  using namespace apgame;
  options_description opt_desc("apgame-client");
  opt_desc.add_options()
    ("help,h", "show help.")
    ("port", value<int>()->default_value(12345), "server port")
    ("host", value<std::string>(), "server host")
    ("user", value<std::string>(), "user name")
    ("room", value<std::string>(), "room name")
  ;

  variables_map vm;
  store(parse_command_line(argc, argv, opt_desc), vm);
  notify(vm);

  if (vm.count("help")) {
    std::cout << opt_desc << std::endl;
    return 1;
  }

  if (!vm.count("port")) {
    std::cout << "server port is not specified" << std::endl;
    return 1;
  }

  if (!vm.count("host")) {
    std::cout << "server host is not specified" << std::endl;
    return 1;
  }
  if (!vm.count("user")) {
    std::cout << "user name is not specified" << std::endl;
    return 1;
  }
  if (!vm.count("room")) {
    std::cout << "room name is not specified" << std::endl;
    return 1;
  }

  SocketClientOption client_option;

  client_option.remotePort(vm["port"].as<int>());
  client_option.remoteAddress(vm["host"].as<std::string>());

  SocketClient socket_client(client_option);
  ReversiPlayer reversi_player(client_option, vm["room"].as<std::string>(), vm["user"].as<std::string>());
  reversi_player.run(myai(reversi_player));
  return 0;
}
