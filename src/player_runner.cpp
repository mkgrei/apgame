#include <boost/program_options.hpp>

int main (int argc, char ** argv);
void main_loop ();

int main (int argc, char ** argv) {

  using namespace boost::program_options;

  options_description opt_desc("apgame-server");
  opt_desc.add_options()
    ("help,h", "show help.")
//     ("host", value<std::string>(), "local server host")
  ;
 
  variables_map vm;
  store(parse_command_line(argc, argv, opt_desc), vm);
  notify(vm);

  if (vm.count("help")) {
    std::cout << opt_desc << std::endl;
    return 1;
  } 

  main_loop();

}

void main_loop () {

  

}
