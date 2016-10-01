#include <apgame/core/context.hpp>
#include <apgame/game/game_client.hpp>

#include <boost/program_options.hpp>

#include <array>
#include <chrono>
#include <iostream>
#include <cstdio>
#include <string>
#include <random>

int main (int argc, char ** argv);
void loop (apgame::game_client & client);
int scene_main (apgame::game_client & client);
int scene_create_room (apgame::game_client & client);

void print_room_info ();
void clear_screen ();
void clear_input_buffer ();

enum scene_index {
  SCENE_MAIN,
  SCENE_CREATE_ROOM,
  SCENE_JOIN_ROOM,
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

  apgame::client client(client_option);
  apgame::game_client game_client;

  client.run([&] (apgame::context & ctx) {
    game_client.init(ctx);
    loop(game_client);
  });
  return 0;
}

// scene management

std::vector<apgame::room_info> room;
int scene;
std::string scene_main_message;

void loop (apgame::game_client & client) {
  scene = SCENE_MAIN;

  while (true) {
    if (!client.call_get_room_info(room)) {
      std::printf("communication error\n");
      std::exit(1);
    }
    switch (scene) {
    case SCENE_MAIN:
      scene = scene_main(client);
      break;
    case SCENE_CREATE_ROOM:
      scene = scene_create_room(client);
      break;
    default:
      scene = SCENE_MAIN;
      break;
    }
  }
}

int scene_main (apgame::game_client & client) {
  clear_screen();
  print_room_info();
  std::printf("%s\n", scene_main_message.data());
  std::printf("choose action\n");
  std::printf("[1] create room\n");
  std::printf("[2] join room\n");
  std::printf("[3] quit\n");

  int action;
  if (!std::scanf("%d", &action)) {
    clear_input_buffer();
    return SCENE_MAIN;
  }

  if (action < 0 || 3 < action) {
    clear_input_buffer();
    return SCENE_MAIN;
  }

  if (action == 1) {
    return SCENE_CREATE_ROOM;
  } else if (action == 2) {
    return SCENE_JOIN_ROOM;
  } else if (action == 3) {
    std::exit(0);
  }
  return SCENE_MAIN;
}

int scene_create_room (apgame::game_client & client) {
  clear_screen();
  print_room_info();

  for (int i = 0; i < apgame::GAME_ID_MAX; ++i) {
    std::printf("[%2d] %s\n", i, apgame::game_id_str[i]);
  }

  int gid;
  std::printf("choose game:");
  if (!std::scanf("%d", &gid)) {
    scene_main_message = "invalid game id";
    clear_input_buffer();
    return SCENE_MAIN;
  }

  if (gid < 0 || gid >= apgame::GAME_ID_MAX) {
    scene_main_message = "invalid game id";
    return SCENE_MAIN;
  }

  std::printf("\n");
  std::printf("room_name: ");
  std::vector<char> buffer(129);
  if (!std::scanf("%128s", const_cast<char *>(&buffer[0]))) {
    return SCENE_MAIN;
  }
  buffer.back() = '\0';
  std::string name(buffer.data());
  if (name.size() == 128) {
    scene_main_message = "too long room_name (max 128)";
    return SCENE_MAIN;
  }

  int error;
  if (!client.call_create_room(name, apgame::game_id(gid), error)) {
    std::printf("communication error\n");
    std::exit(1);
  }
  switch (error) {
  case 0:
    scene_main_message = "room created";
    break;
  case 1:
    scene_main_message = "the name is used already";
    break;
  case 2:
    scene_main_message = "unknown game";
    break;
  default:
    break;
  }
  return SCENE_MAIN;
}

void print_room_info () {
  std::printf("------------ room info ------------\n");
  for (int i = 0; i < int(room.size()); ++i) {
    std::printf("[%5d][%16s] %s\n", i, apgame::game_id_str[room[i].id], room[i].name.data());
  }
  std::printf("-----------------------------------\n");
}

void clear_screen () {
  std::printf("\e[1;1H\e[2J");
}

void clear_input_buffer () {
  char c;
  while ((c = std::getchar()) != '\n') {}
}
