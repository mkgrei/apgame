#pragma once

#include <apgame/core/context.hpp>

#include <atomic>
#include <mutex>
#include <memory>

namespace apgame {

struct game_player {

  game_player () noexcept = default;

  virtual void run (context & ctx) = 0;

};

}
