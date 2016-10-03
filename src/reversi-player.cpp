#include <apgame/socket/SocketClient.hpp>
#include <apgame/game/GameClient.hpp>
#include <apgame/reversi/ReversiClient.hpp>

#include <boost/program_options.hpp>

#include <array>
#include <chrono>
#include <iostream>
#include <string>
#include <random>

namespace apgame {

struct ReversiPlayer {

  ReversiPlayer (SocketClientOption const & opt, std::string room, std::string user) 
  : client_(opt)
  , room_(std::move(room))
  , user_(std::move(user)) {
  }

  template <class Handler>
  void run (Handler && handler) {
    client_.run([&] (SocketContext & socket_context) {
      proc_game(socket_context);
      proc_reversi(socket_context, handler);
    });
  }

private:
  apgame::SocketClient client_;
  std::string room_;
  std::string user_;

  void proc_game (SocketContext & socket_context) {
    GameClient game_client(socket_context);
    int error;
    if (!game_client.callNegotiate(user_, error)) {
      std::printf("failed to call negotiate\n"); 
      std::exit(1);
    }
    if (error != 0) {
      std::printf("failed to negotiate\n");
      std::exit(1);
    }

    if (!game_client.callCreateRoom(room_, GameID::GAME_ID_REVERSI, error)) {
      std::printf("failed to call createRoom\n");
      std::exit(1);
    }
    if (error != 0 && error != 1) {
      std::printf("failed to create room\n");
      std::exit(1);
    }

    if (!game_client.callJoinRoom(GameID::GAME_ID_REVERSI, room_, error)) {
      std::printf("failed to call joinRoom\n");
      std::exit(1);
    }

    if (error != 0) {
      std::printf("failed to join room\n");
      std::exit(1);
    }
  };

  template <class Handler>
  void proc_reversi (SocketContext & socket_context, Handler && handler) {
    ReversiClient reversi_client(socket_context);

    int error;
    if (!reversi_client.callJoin(error)) {
      std::printf("failed to call join\n");
      std::exit(1);
    }
    if (error != 0) {
      std::printf("failed to join\n");
      std::exit(1);
    }

    while (true) {
      ReversiStatus status;
      if (!reversi_client.callGetStatus(status)) {
        std::printf("failed to get status\n");
        std::exit(1);
      }
      if (status != REVERSI_STATUS_BEFORE_GAME) {
        break;
      }
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    ReversiStone color;
    if (!reversi_client.callGetColor(color)) {
      std::printf("failed to get color\n");
      std::exit(1);
    }
    std::printf("YOU ARE %s\n", color == REVERSI_STONE_BLACK ? "BLACK" : "WHITE");

    while (true) {
      ReversiStatus status;
      if (!reversi_client.callGetStatus(status)) {
        std::printf("failed to get status\n");
        std::exit(1);
      }

      int x, y;
      if (color == REVERSI_STONE_BLACK && status == REVERSI_STATUS_BLACK_TURN) {
        if (!reversi_client.callGetLastStone(x, y)) {
          std::printf("failed to get last stone\n");
          break;
        }
      } else if (color == REVERSI_STONE_WHITE && status == REVERSI_STATUS_WHITE_TURN) {
        if (!reversi_client.callGetLastStone(x, y)) {
          std::printf("failed to get last stone\n");
          break;
        }
      } else if (status == REVERSI_STATUS_AFTER_GAME) {
        after_game(socket_context);
      } else {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        continue;
      }

      handler(color, x, y);

      int error;
      if (!reversi_client.callPutStone(x, y, error)) {
        std::printf("failed to get last stone\n");
        std::exit(1);
      }
      if (error != 0) {
        std::printf("invalid put\n");
      }
    }
  }

  void after_game (SocketContext & socket_context) {

  }
};

}

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
  player.run([&](ReversiStone color, int & x, int & y) {
    std::string line;
    if (!std::getline(std::cin, line)) {
      std::exit(1);
    }
    x = std::stoi(line);
    if (!std::getline(std::cin, line)) {
      std::exit(1);
    }
    y = std::stoi(line);

  });
  return 0;
}
