#pragma once

#include <array>

namespace apgame {

enum GameID : int {
  GAME_ID_REVERSI = 0,
  GAME_ID_MAX
};

char const * gameIDStr (GameID id) noexcept {
  static std::array<char const *, GAME_ID_MAX> map {
    "REVERSI"
  };
  return map[int(id)];
}

enum GameCommand: int {
  GAME_COMMAND_CREATE_ROOM = 0,
  GAME_COMMAND_JOIN_ROOM,
  GAME_COMMAND_GET_ROOM_INFO,
  GAME_COMMAND_MAX
};

char const * gameCommandStr (GameCommand cmd) noexcept {
  static std::array<char const *, GAME_COMMAND_MAX> map {
    "CREATE_ROOM",
    "JOIN_ROOM",
    "GET_ROOM_INFO"
  };
  return map[int(cmd)];
}

}
