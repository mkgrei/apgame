#pragma once

#include <apgame/game/enum.hpp>
#include <apgame/game/User.hpp>

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

  virtual ~Game () noexcept = default;

/**
 *  @return
 *  true: success
 *  false: a player name is used
 */
  bool addUser (std::string const & name) {
    LOG_DEBUG("add_user name = ", name);
    std::lock_guard<std::mutex> lock(mtx_);
    if (user_.count(name) > 0) {
      LOG_ERROR("player name is used");
      return false;
    }
    user_.emplace(name);
  }

/**
 *  @return
 *  true: success
 *  false: a player name is not used
 */
  bool delUser (std::string const & name) {
    LOG_DEBUG("del_user name = ", name);
    std::lock_guard<std::mutex> lock(mtx_);
    if (user_.erase(name) == 0) {
      LOG_ERROR("unknown player name");
      return false;
    }
    return true;
  }

protected:

  std::mutex mtx_;
  std::unordered_set<std::string> user_;
};

}
