#include <apgame/reversi/ReversiPlayer.hpp>
#include <boost/program_options.hpp>

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

  std::mt19937 random_engine;

  myai () {
    std::random_device random_device;
    random_engine.seed(random_device());
  }

  void operator() (int color, std::array<apgame::ReversiStone, 64> & board, int & x, int & y) {
    x = std::uniform_int_distribution<int>(0, 7)(random_engine);
    y = std::uniform_int_distribution<int>(0, 7)(random_engine);
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
  player.run(myai());
  return 0;
}
