#include <apgame/game/GameClient.hpp>
#include <apgame/socket/SocketContext.hpp>
#include <apgame/socket/SocketClient.hpp>
#include <apgame/socket/SocketClientOption.hpp>

#include <boost/program_options.hpp>

#include <array>
#include <chrono>
#include <iostream>
#include <cstdio>
#include <string>
#include <random>

using namespace apgame;

struct Scene {

  enum SceneIndex {
    SCENE_MAIN,
    SCENE_CREATE_ROOM,
    SCENE_JOIN_ROOM,
  };

  GameClient client;

  std::vector<RoomInfo> room;
  int scene;
  std::string main_message;

  Scene()
  : scene(SCENE_MAIN) {
  }

  Scene & operator() (SocketContext & ctx) {
    client.init(ctx);
    while (true) {
      if (!client.callGetRoomInfo(room)) {
        std::printf("communication error\n");
        std::exit(1);
      }
      switch (scene) {
      case SCENE_MAIN:
        sceneMain();
        break;
      case SCENE_CREATE_ROOM:
        sceneCreateRoom();
        break;
      case SCENE_JOIN_ROOM:
        sceneJoinRoom();
        break;
      default:
        scene = SCENE_MAIN;
        break;
      }
    }
    return *this;
  }

  void sceneMain () {
    clearScreen();
    printRoomInfo();
    std::printf("%s\n", main_message.data());
    std::printf("choose action\n");
    std::printf("[1] create room\n");
    std::printf("[2] join room\n");
    std::printf("[3] quit\n");
  
    int action;
    if (!std::scanf("%d", &action)) {
      clearInputBuffer();
      scene = SCENE_MAIN;
      return;
    }
  
    if (action < 0 || 3 < action) {
      clearInputBuffer();
      scene = SCENE_MAIN;
      return;
    }
  
    if (action == 1) {
      scene = SCENE_CREATE_ROOM;
      return;
    } else if (action == 2) {
      scene = SCENE_JOIN_ROOM;
      return;
    } else if (action == 3) {
      std::exit(0);
    }
    scene = SCENE_MAIN;
    return;
  }

  void sceneCreateRoom () {
    clearScreen();
    printRoomInfo();
  
    for (int i = 0; i < GAME_ID_MAX; ++i) {
      std::printf("[%2d] %s\n", i, gameIDStr(GameID(i)));
    }
  
    int game_id;
    std::printf("choose game:");
    if (!std::scanf("%d", &game_id)) {
      main_message = "invalid game id";
      clearInputBuffer();
      scene = SCENE_MAIN;
      return;
    }
  
    if (game_id < 0 || apgame::GAME_ID_MAX <= game_id) {
      main_message = "invalid game id";
      scene = SCENE_MAIN;
      return;
    }
  
    std::printf("\n");
    std::printf("room_name: ");
    std::vector<char> buffer(129, '\0');
    if (!std::scanf("%128s", const_cast<char *>(&buffer[0]))) {
      scene = SCENE_MAIN;
      return;
    }
  
    buffer.back() = '\0';
    std::string name(buffer.data());
    if (name.size() == 128) {
      main_message = "too long room_name (max 128)";
      scene = SCENE_MAIN;
      return;
    }
  
    int error;
    if (!client.callCreateRoom(name, GameID(game_id), error)) {
      std::printf("communication error\n");
      std::exit(1);
    }
    switch (error) {
    case 0:
      main_message = "room created";
      break;
    case 1:
      main_message = "the name is used already";
      break;
    case 2:
      main_message = "unknown game";
      break;
    default:
      break;
    }
    scene = SCENE_MAIN;
    return;
  }

  void sceneJoinRoom () {
    clearScreen();
    printRoomInfo();
  
    int room_id;
    std::printf("choose room:");
    if (!std::scanf("%d", &room_id)) {
      main_message = "invalid room id";
      clearInputBuffer();
      scene = SCENE_MAIN;
      return;
    }
  
    if (room_id < 0 || int(room.size()) <= room_id) {
      main_message = "invalid room id";
      scene = SCENE_MAIN;
      return;
    }
  
    if (!client.callJoinRoom(room[room_id].id, room[room_id].name)) {
      main_message = "failed to join room";
      scene = SCENE_MAIN;
      return;
    }
  
    main_message = "joined!";
    scene = SCENE_MAIN;
    return;
  }
  
  void printRoomInfo () {
    std::printf("------------ room info ------------\n");
    for (int i = 0; i < int(room.size()); ++i) {
      std::printf("[%5d][%16s] %s\n", i, gameIDStr(room[i].id), room[i].name.data());
    }
    std::printf("-----------------------------------\n");
  }
  
  void clearScreen () {
    std::printf("\e[1;1H\e[2J");
  }
  
  void clearInputBuffer () {
    char c;
    while ((c = std::getchar()) != '\n') {}
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

  SocketClientOption opt;

  opt.remotePort(vm["port"].as<int>());
  opt.remoteAddress(vm["host"].as<std::string>());

  SocketClient client(opt);
  client.run([&] (SocketContext & ctx) {
    Scene scene;
    scene(ctx);
  });
  return 0;
}

