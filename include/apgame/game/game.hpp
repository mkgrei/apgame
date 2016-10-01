#pragma once

#include <apgame/core/context.hpp>

#include <apgame/game/game_enum.hpp>

#include <atomic>
#include <mutex>
#include <vector>

namespace apgame {

struct game {

  game (std::string && room_name)
  : room_name(std::move(room_name)) {
  }

  const std::string room_name;

  virtual game_id get_game_id () const noexcept = 0;  
  virtual char const * get_game_name () const noexcept = 0;

  virtual ~game () noexcept = default;

protected:

  std::atomic_int num_player_;
  std::atomic_int num_observer_;

};

}
