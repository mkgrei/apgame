#include <apgame/game/GameManager.hpp>
#include <apgame/socket/SocketContext.hpp>
#include <apgame/socket/SocketServer.hpp>

#include <boost/program_options.hpp>

int main (int argc, char ** argv) {

  using namespace boost::program_options;
  using namespace apgame;

  options_description opt_desc("apgame-server");
  opt_desc.add_options()
    ("help,h", "show help.")
    ("host", value<std::string>(), "local server host")
    ("port", value<int>()->default_value(12345), "local server port")
  ;
 
  variables_map vm;
  store(parse_command_line(argc, argv, opt_desc), vm);
  notify(vm);

  if (vm.count("help")) {
    std::cout << opt_desc << std::endl;
    return 1;
  } 

  SocketServerOption opt;
  opt 
    .localAddress(vm["host"].as<std::string>())
    .localPort(vm["port"].as<int>())
    .maxConnection(2)
  ;

  SocketServer server(opt);
  server.run(GameManager());
}
