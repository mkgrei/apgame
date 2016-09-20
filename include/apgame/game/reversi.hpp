#pragma once

#include <apgame/game/game.hpp>
#include <apgame/game/game_enum.hpp>
#include <apgame/game/reversi_enum.hpp>

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
    status_ = REVERSI_STATUS_BEFORE_GAME;
    player_black_ = 0;
    player_white_ = 0;
  }

  game_id get_game_id () const noexcept override {
    return GAME_ID_REVERSI;
  }

  char const * get_game_name () const noexcept override {
    return "reversi";
  }

/**
 *  @details
 *  if token == 0, new token is published.
 *  if token != 0, user specified token is used.
 *
 *  In success, token is non-zero integer.
 */
  bool api_add_user (unsigned int & token) {
    std::lock_guard<std::mutex> lock(mtx_);
    LOG_DEBUG("add_user ...\n");
    for (int i = 0; i < 16; ++i) {
      unsigned int tok = random_engine_();
      if (tok != 0 && userset_.count(tok) == 0) {
        userset_.emplace(tok, user());
        token = tok;
        LOG_DEBUG("add_user ... ok ... token = %08x\n", tok);
        return true;
      }
    }
    LOG_DEBUG("add_user ... fail\n");
    return false;
  }

/**
 *  @details
 *  require auth.
 */
  bool api_make_player (int token, bool & is_black) {
    std::lock_guard<std::mutex> lock(mtx_);

    LOG_DEBUG("make_player ... token = %u\n", token);
    if (!check_user(token)) {
      LOG_DEBUG("make_player ... fail\n");
      return false;
    }

    if (status_ != REVERSI_STATUS_BEFORE_GAME) {
      LOG_DEBUG("make_player ... fail\n");
      return false;
    }

    if (player_black_ == 0) {
      player_black_ = token;
      is_black = true;
    } else if (player_white_ == 0) {
      player_white_ = token;
      is_black = false;
    } else {
      LOG_DEBUG("make_player ... fail\n");
      return false;
    }

    if (player_black_ == 0 || player_white_ == 0) {
      LOG_DEBUG("make_player ... ok ... is_black = %s\n", is_black ? "true" : "false");
      return true;
    }

    status_ = REVERSI_STATUS_BLACK_TURN;
    LOG_DEBUG("make_player ... ok ... is_black = %s\n", is_black ? "true" : "false");

    return true;
  }

  bool api_get_game_status (reversi_status & status) const noexcept {
    std::lock_guard<std::mutex> lock(mtx_);
    status = status_;
    return true;
  }

  bool api_get_board (std::array<reversi_stone, 64> & board) const noexcept {
    std::lock_guard<std::mutex> lock(mtx_);
    board = board_;
    return true;
  }

  bool api_put_stone (unsigned int token, int x, int y) {
    std::lock_guard<std::mutex> lock(mtx_);

    if (player_black_ == token) {
      if (status_ != REVERSI_STATUS_BLACK_TURN) {
        return false;
      }
      status_ = REVERSI_STATUS_WHITE_TURN;
      return true;
    } else if (player_white_ == token) {
      if (status_ != REVERSI_STATUS_WHITE_TURN) {
        return false;
      }
      status_ = REVERSI_STATUS_BLACK_TURN;
      return true;
    } else {
      return false;
    }
  }

  bool api_close (int token) {
    std::lock_guard<std::mutex> lock(mtx_);
    if (!check_user(token)) {
      return false;
    }
    userset_[token].active = false;
    return true;
  }

private:

  struct user {

    user () noexcept {
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

  std::mt19937_64 random_engine_;

  mutable std::mutex mtx_;
  std::unordered_map<unsigned int, user> userset_;
  unsigned int player_black_;
  unsigned int player_white_;

  std::array<reversi_stone, 64> board_;
  reversi_status status_;

  bool check_user (unsigned int token) {
    return userset_.count(token) == 0;
  }
};

}
