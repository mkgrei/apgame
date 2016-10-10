#pragma once

#include <apgame/game/enum.hpp>
#include <apgame/game/User.hpp>

#include <apgame/core/logging.hpp>
#include <apgame/socket/SocketContext.hpp>

#include <mutex>
#include <unordered_set>
#include <vector>

namespace apgame {

struct GameContext;

struct Game {

  Game () {
  }

  virtual GameID gameID () const noexcept = 0;
  virtual char const * gameName () const noexcept = 0;
  virtual std::size_t getMaxUser () const noexcept = 0;

  virtual bool join (User * user) = 0;
  virtual bool initialize () = 0;

  virtual void run (GameContext & game_context) = 0;
  virtual ~Game () noexcept = default;

protected:
  std::mutex mtx_;
};

}
