#include <apgame/core/context.hpp>
#include <apgame/core/server.hpp>
#include <apgame/core/server_option.hpp>
#include <apgame/game/game_manager.hpp>

#include <boost/program_options.hpp>

int main (int argc, char ** argv) {

  using namespace boost::program_options;

  options_description opt_desc("apgame-server");
  opt_desc.add_options()
    ("help,h", "show help.")
    ("local-port", value<int>()->default_value(12345), "local server port")
  ;
 
  variables_map vm;
  store(parse_command_line(argc, argv, opt_desc), vm);
  notify(vm);

  if (vm.count("help")) {
    std::cout << opt_desc << std::endl;
    return 1;
  } 

  apgame::server_option server_option;
  server_option 
    .local_port(vm["local-port"].as<int>())
    .max_connection(2)
  ;

  apgame::server server(server_option);
  server.run(apgame::game_manager());
}
