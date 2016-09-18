#include <apgame/core/context.hpp>

#include <apgame/core/server.hpp>
#include <apgame/core/server_option.hpp>

#include <apgame/game/game_manager.hpp>

int main (int argc, char ** argv) {

  apgame::server_option opt;
  opt
    .local_port(12345)
    .max_connection(2)
  ;

  apgame::server server(opt);
  server.run(apgame::game_manager());
}
