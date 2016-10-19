#pragma once

#include <array>

namespace apgame {

enum RoomCommand : int {
  ROOM_COMMAND_JOIN_ROOM = 0,
  ROOM_COMMAND_EXIT,
  ROOM_COMMAND_MAX
};

char const * RoommCommandStr (RoomCommand cmd) noexcept {
  static std::array<char const *, ROOM_COMMAND_MAX> map {
    "JOIN_ROOM",
    "EXIT"
  };
  return map[int(cmd)];
}

}
