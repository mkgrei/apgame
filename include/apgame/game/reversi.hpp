#pragma once

#include <apgame/game/game.hpp>

#include <atomic>
#include <ctime>
#include <mutex>
#include <unordered_map>
#include <random>

namespace apgame {

struct reversi : public game {

  reversi () {
    std::random_device device;
    random_engine_.seed(device());
    set_game_status(BEFORE_GAME);
    player_[0] = 0;
    player_[1] = 0;
    black_ = 0;
    white_ = 0;
  }

  enum game_status : int {
    BEFORE_GAME,
    BLACK_TURN,
    WHITE_TURN,
    AFTER_GAME,
    ERROR
  };

  enum request : int {
    INIT,
    MAKE_PLAYER,
    GET_GAME_STATUS,
    GET_USER_INFO
  };

  char const * get_game_name () const noexcept override {
    return "reversi";
  }

  bool make_player (int token) {
    if (get_game_status() != game_status::BEFORE_GAME) {
      return false;
    }
    std::lock_guard<std::mutex> lock(user_mtx_);
    if (!check_user(token)) {
      return false;
    }
    for (int & player : player_) {
      if (player == 0) {
        player = token;
        return true;
      }
    }
    return false;
  }

  game_status get_game_status () const noexcept {
    return game_status(int(status_));
  }

  bool generate_user (int & token) {
    std::lock_guard<std::mutex> lock(user_mtx_);
    for (int i = 0; i < 16; ++i) {
      int tok = random_engine_();
      if (tok != 0 && user_.count(tok) == 0) {
        user_.emplace(tok, user_data());
        token = tok;
        return true;
      }
    }
    return false;
  }

  bool check_token (int token) {
    return token_.count(token) > 0;
  }

private:

  struct user_data {

    user_data () noexcept {
      last_comm_timestamp = std::time(NULL);
      active = true;
      is_player = false;
      is_black = false;
    }

    bool active;
    bool is_player;
    bool is_black;
    std::time_t last_comm_timestamp;
  };

  std::atomic_int status_;
  std::mt19937_64 random_engine_;

  std::mutex user_mtx_;
  std::unordered_map<int, user_data> user_;

  std::array<int, 2> player_;
  int black_;
  int white_;

  void set_game_status (game_status const & s) noexcept {
    status_ = int(s);
  }


};

}
