#include <apgame/game/ReversiPlayer.hpp>
#include <apgame/socket/SocketContext.hpp>

#include <boost/program_options.hpp>

#include <array>
#include <chrono>
#include <iostream>
#include <string>
#include <random>

struct myplayer {
  void operator() (bool is_black, std::array<apgame::reversi_stone, 64> const & board, int & x, int & y) {
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
};

int main (int argc, char ** argv) {

  using namespace boost::program_options;
  using namespace apgame;
  options_description opt_desc("apgame-client");
  opt_desc.add_options()
    ("help,h", "show help.")
    ("port", value<int>()->default_value(12345), "server port")
    ("host", value<std::string>(), "server host")
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

  ClientOption client_option;

  client_option.remote_port(vm["port"].as<int>());
  client_option.remote_address(vm["host"].as<std::string>());

  ReversiPlayer player(client_option, "mygame");
  player.run(myplayer());
  return 0;
}
