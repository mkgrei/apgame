#pragma once

#include <array>

namespace apgame {

enum UserCommand : int {
  USER_COMMAND_JOIN_USER = 0,
  USER_COMMAND_EXIT,
  USER_COMMAND_MAX
};

char const * UsermCommandStr (UserCommand cmd) noexcept {
  static std::array<char const *, USER_COMMAND_MAX> map {
    "JOIN_USER",
    "EXIT"
  };
  return map[int(cmd)];
}

}
