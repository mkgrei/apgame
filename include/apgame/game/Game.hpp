#pragma once

#include <apgame/game/enum.hpp>
#include <apgame/game/User.hpp>
#include <apgame/game/GameContext.hpp>

#include <apgame/core/logging.hpp>
#include <apgame/socket/SocketContext.hpp>

#include <mutex>
#include <unordered_set>
#include <vector>

namespace apgame {

struct Game {

  Game (std::string room_name)
  : room_name(std::move(room_name)) {
  }

  const std::string room_name;

  virtual GameID gameID () const noexcept = 0;  
  virtual char const * gameName () const noexcept = 0;
  virtual void run (GameContext & game_context) = 0;

  virtual ~Game () noexcept = default;

protected:
  std::mutex mtx_;
};

}
