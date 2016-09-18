#pragma once

#include <apgame/core/context.hpp>

#include <atomic>
#include <mutex>

namespace apgame {

struct game {

  game () noexcept = default;

  virtual char const * get_game_name () const noexcept = 0;

  virtual ~game () noexcept = default;

protected:

  std::atomic_int num_player_;
  std::atomic_int num_observer_;
};

}
