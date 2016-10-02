#pragma once

#include <apgame/socket/SocketContext.hpp>

#include <atomic>
#include <mutex>
#include <memory>

namespace apgame {

struct game_api {

  game_api () noexcept = default;

  virtual void run () = 0;

};

}
