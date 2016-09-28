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
    LOG_DEBUG("add_user: token = %u\n", token);
    for (int i = 0; i < 16; ++i) {
      unsigned int tok = random_engine_();
      if (userset_.count(tok) == 0) {
        userset_.emplace(tok, user()); 
        token = tok;
        LOG_DEBUG("add_user: ok, publish new token = %u\n", token);
        return true;
      }
    }
    LOG_ERROR("add_user: fail, hash table is too dense?\n");
    return false;
  }

/**
 *  @details
 */
  bool api_make_player (unsigned int token, bool & is_black) {
    std::lock_guard<std::mutex> lock(mtx_);

    LOG_DEBUG("make_player: token = %u\n", token);
    if (!check_user(token)) {
      LOG_ERROR("make_player: fail, unknown token = %u\n", token);
      return false;
    }

    if (status_ != REVERSI_STATUS_BEFORE_GAME) {
      LOG_ERROR("make_player: fail, invalid status = %d\n", status_);
      return false;
    }

    if (player_black_ == 0) {
      player_black_ = token;
      is_black = true;
      userset_[token].is_black = true;
    } else if (player_white_ == 0) {
      player_white_ = token;
      is_black = false;
      userset_[token].is_white = true;
    } else {
      LOG_ERROR("make_player: fail, there are already two players\n");
      return false;
    }

    if (player_black_ == 0 || player_white_ == 0) {
      LOG_DEBUG("make_player: ok, is_black = %s\n", is_black ? "true" : "false");
      return true;
    }

    init_board();
    status_ = REVERSI_STATUS_BLACK_TURN;
    LOG_DEBUG("make_player: ok, is_black = %s\n", is_black ? "true" : "false");
    LOG_INFO("game started\n");
    return true;
  }

  bool api_get_game_status (reversi_status & status) const noexcept {
    std::lock_guard<std::mutex> lock(mtx_);
    LOG_DEBUG("get_game_status: status = %d\n", status);
    status = status_;
    LOG_DEBUG("get_game_status: ok, status = %d\n", status);
    return true;
  }

  bool api_get_board (std::array<reversi_stone, 64> & board) const noexcept {
    LOG_DEBUG("get_board:\n");
    std::lock_guard<std::mutex> lock(mtx_);
    board = board_;
    LOG_DEBUG("get_board: ok\n");
    return true;
  }

  bool api_put_stone (unsigned int token, int x, int y) {
    std::lock_guard<std::mutex> lock(mtx_);
    LOG_DEBUG("put_stone: token = %u, x = %d, y = %d\n", token, x, y);

    bool is_black = (player_black_ == token ? true : false);
    bool is_white = (player_white_ == token ? true : false);

    if (!is_black && !is_white) {
      return false;
    }

    LOG_DEBUG("put_stone: player is %s\n", is_black ? "black" : "false");

    if (is_black && status_ != REVERSI_STATUS_BLACK_TURN) {
      LOG_ERROR("put_stone: fail, invalid turn = %d\n", status_);
      return false;
    }
    if (is_white && status_ != REVERSI_STATUS_WHITE_TURN) {
      LOG_ERROR("put_stone: fail, invalid turn = %d\n", status_);
      return false;
    }

    if (!check_put_stone(is_black, x, y)) {
      LOG_ERROR("put_stone: fail, invalid put\n");
      return false;
    }
    LOG_DEBUG("put_stone: ok\n");
    status_ = is_black ? REVERSI_STATUS_WHITE_TURN : REVERSI_STATUS_BLACK_TURN;
    return true;
  }

  bool api_close (int token) {
    LOG_DEBUG("api_close: token = %u\n", token);
    std::lock_guard<std::mutex> lock(mtx_);
    if (!check_user(token)) {
      LOG_DEBUG("api_close: fail, unknown user\n", token);
      return false;
    }
    auto & user = userset_[token];
    if (!user.is_black && !user.is_white) {
      userset_.erase(token);
    } else if (user.is_black && status_ == REVERSI_STATUS_BEFORE_GAME) {
      player_black_ = 0;
      userset_.erase(token);
    } else if (user.is_white && status_ == REVERSI_STATUS_BEFORE_GAME) {
      player_white_ = 0;
      userset_.erase(token);
    }

    LOG_DEBUG("api_close: ok\n");
    return true;
  }

private:

  struct user {

    user () noexcept {
      last_comm_timestamp = std::time(NULL);
      is_white = false;
      is_black = false;
    }

    bool is_black;
    bool is_white;
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
    return userset_.count(token) > 0;
  }

  void init_board() {
    for (reversi_stone & stone : board_) {
      stone = reversi_stone::EMPTY;
    }
    board_[3 + 8 * 3] = reversi_stone::WHITE;
    board_[4 + 8 * 4] = reversi_stone::WHITE;
    board_[3 + 8 * 4] = reversi_stone::BLACK;
    board_[4 + 8 * 3] = reversi_stone::BLACK;
  }

  bool check_put_stone (bool is_black, int x, int y) {
    if (!(0 <= x && x < 8)) {
      return false;
    }
    if (!(0 <= y && y < 8)) {
      return false;
    }
    if (board_[x + 8 * y] != reversi_stone::EMPTY) {
      return false;
    }

    std::array<int, 16> K{
      1, 0,
      1, 1,
      0, 1,
      -1, 1,
      -1, 0,
      -1, -1,
      0, -1,
      1, -1
    };

    reversi_stone my = is_black ? reversi_stone::BLACK : reversi_stone::WHITE;
    reversi_stone other = is_black ? reversi_stone::WHITE : reversi_stone::BLACK;

    auto board = board_;

    bool flag = false;
    for (int i = 0; i < 8; ++i) {
      int kx = K[2 * i];
      int ky = K[2 * i + 1];
      int x1 = x + kx;
      int y1 = y + ky;

      if (!(0 <= x1 && x1 < 8 && 0 <= y1 && y1 < 8)) {
        continue;
      }
      if (board[x1 + 8 * y1] != other) {
        continue;
      }
      x1 += kx;
      y1 += ky;

      int j = 2;
      while (0 <= x1 && x1 < 8 && 0 <= y1 && y1 < 8) {
        if (board_[x1 + 8 * y1] == reversi_stone::EMPTY) {
          break;
        }
        if (board_[x1 + 8 * y1] == my) {
          for (int l = 0; l < j; ++l) {
            int x2 = x + l * kx;
            int y2 = y + l * ky;
            board[x2 + 8 * y2] = my;
          }
          flag = true;
          break;
        }
        ++j;
        x1 += kx;
        y1 += ky;
      }
    }
    if (flag) {
      board_ = board;
      print_board();
    }
    return flag;
  }

  void print_board() {
    for (int y = 0; y < 8; ++y) {
      for (int x = 0; x < 8; ++x) {
        reversi_stone stone = board_[x + 8 * y];
        if (stone == reversi_stone::EMPTY) {
          std::cout << '.';
        }
        if (stone == reversi_stone::WHITE) {
          std::cout << 'W';
        }
        if (stone == reversi_stone::BLACK) {
          std::cout << 'B';
        }
      }
      std::cout << std::endl;
    }
  }

};

}
