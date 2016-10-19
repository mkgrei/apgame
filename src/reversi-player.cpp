#include <apgame/reversi/ReversiPlayer.hpp>
#include <boost/program_options.hpp>
#include <lh-ai/ai.hpp>

/**
 *  color =  1: YOU ARE BLACK
 *  color = -1: YOU ARE WHITE
 *
 *  @input
 *  enemy put stone on (x, y)
 *  if this is the first turn, (x, y) = (-1, -1)
 *
 *  @output
 *  you put stone on (x, y)
 *
 *  if you have no choice
 *  ---------------------
 *  even if you have no choice to put stone, the function is called, and you can check the opponent's activity.
 *  if you have no choice, x and y which you specified are ignored.
 *  then specifying invalid coordinate like (-1, -1) is safe.
 *
 *  board
 *  -----
 *  board array is Z order.
 *   0: empty
 *   1: BLACK
 *  -1: WHITE
 */
struct myai {
  AI* smart_ai = new RandomAI();
  apgame::ReversiPlayer & player;
  myai (apgame::ReversiPlayer & player) : player(player) {
  }
  bool is_first_time = true;
  void operator() (bool is_black, std::array<apgame::ReversiStone, 64> const & board, int & x, int & y) {
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
  using namespace apgame;

  options_description opt_desc("client-human");
  opt_desc.add_options()
    ("help,h", "show help.")
    ("port", value<int>()->default_value(12345), "server port")
    ("host", value<std::string>()->required(), "server host")
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

  SocketClientOption opt;

  opt.remotePort(vm["port"].as<int>());
  opt.remoteAddress(vm["host"].as<std::string>());

  auto user = vm["user"].as<std::string>();
  auto room = vm["room"].as<std::string>();

  ReversiPlayer player(opt, std::move(room), std::move(user));
  player.run(myai(player));
  return 0;
}
