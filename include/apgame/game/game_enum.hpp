#pragma once

#include <array>

namespace apgame {

enum game_id : int {
  GAME_ID_REVERSI = 0,
  GAME_ID_MAX
};

std::array<char const *, GAME_ID_MAX> game_id_str {
  "REVERSI"
};

enum game_command : int {
  GAME_COMMAND_CREATE_ROOM = 0,
  GAME_COMMAND_JOIN_ROOM,
  GAME_COMMAND_GET_ROOM_INFO,
  GAME_COMMAND_MAX
};

std::array<char const *, GAME_COMMAND_MAX> game_command_str {
  "CREATE_ROOM",
  "JOIN_ROOM",
  "GET_ROOM_INFO"
};

}
