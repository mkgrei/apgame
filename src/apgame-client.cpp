#include <apgame/core/context.hpp>
#include <apgame/game/reversi_player.hpp>
#include <lh-ai/ai.hpp>

#include <boost/program_options.hpp>

#include <array>
#include <chrono>
#include <iostream>
#include <string>
#include <random>

struct myplayer {
  AI* smart_ai = new RandomAI();
  bool is_first_time = true;
  void operator() (bool is_black, std::array<apgame::reversi_stone, 64> const & board, int & x, int & y) {
    std::array<char, 64> char_board;
    for (unsigned i=0; i<board.size(); i++) {
      if (board[i]==0) char_board[i]=0;
      else char_board[i]=1;
    }
    if (is_black && is_first_time) {
      is_first_time = false;
    } else {
      this->smart_ai->setOpponentMove(char_board);
    }
    Point p;
    p = this->smart_ai->move();
    while (p.x==0 && p.y==0) {
      this->smart_ai->setOpponentMove(char_board);
      p = this->smart_ai->move();
    }
    x = p.x;
    y = p.y;
    return;
  }
};

int main (int argc, char ** argv) {
  using namespace boost::program_options;

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

  apgame::client_option client_option;

  client_option.remote_port(vm["port"].as<int>());
  client_option.remote_address(vm["host"].as<std::string>());

  apgame::reversi_player player(client_option, "mygame");
  player.run(myplayer());
  return 0;
}
