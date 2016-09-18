#pragma once

#include <apgame/game/game.hpp>

#include <atomic>

namespace apgame {

struct reversi : public game {

  reversi () {
    set_status(BEFORE_START);
  }

  enum status : int {
    BEFORE_START,
    ERROR
  };

  enum request : int {
    Init,
    GetServerInfo
  };

  char const * get_game_name () const noexcept override {
    return "reversi";
  }

/**
 *  @details
 *    thread-safe: yes
 */
  status get_status () const noexcept {
    return status(int(status_));
  }

/**
 *  @details
 *    thread-safe: yes
 */
  void set_status (status const & s) noexcept {
    status_ = int(s);
  }

private:
  std::atomic_int status_;
};

}
