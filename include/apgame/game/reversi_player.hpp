#pragma once

#include <apgame/core/client.hpp>
#include <apgame/core/client_option.hpp>
#include <apgame/game/game_enum.hpp>
#include <apgame/game/reversi.hpp>
#include <apgame/game/reversi_enum.hpp>
#include <apgame/game/reversi_client.hpp>



namespace apgame {

struct reversi_player {

  reversi_player (client_option const & opt, std::string board_name)
  : socket_{opt}
  , board_name_{board_name}
  , token_{0} {
  }

  template <class Handler>
  void run (Handler && handler) {
    socket_.run([&] (context & ctx) {
      gamer_.init(ctx);

      if (!gamer_.call_join_game(GAME_ID_REVERSI, board_name_)) {
        LOG_ERROR("join_game ... fail\n");
        return;
      }

      if (!gamer_.call_add_user(token_)) {
        LOG_ERROR("add_user ... fail\n");
        return; 
      }
      gamer_.call_command(12345);
      return;

      if (!gamer_.call_make_player(is_black_)) {
        LOG_ERROR("make_player ... fail\n");
        return; 
      }

      while (true) {
        reversi_status status;
        if (!gamer_.call_get_game_status(status)) {
          LOG_ERROR("get_game_status ... fail\n");
          return;
        }
        if (status != REVERSI_STATUS_BEFORE_GAME) {
          break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); 
      }

      while (true) {
        reversi_status status;
        if (!gamer_.call_get_game_status(status)) {
          LOG_ERROR("get_game_status ... fail\n");
          return;
        }

        if (status == REVERSI_STATUS_AFTER_GAME) {
          break;
        }

        if (is_black() && status != REVERSI_STATUS_BLACK_TURN) {
          continue;
        }

        if (is_white () && status != REVERSI_STATUS_WHITE_TURN) {
          continue;
        }

        if (!gamer_.call_get_board(board_)) {
          LOG_ERROR("call_get_board ... fail\n");
          return;
        }

        int x, y;
        handler(is_black(), board(), x, y);

        bool success;
        if (!gamer_.call_put_stone(x, y, success)) {
          LOG_ERROR("put_stone ... fail\n");
          return;
        }
        if (!success) {
          LOG_ERROR("put_stone ... fail\n");
          return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(300)); 
      }
      LOG_INFO("finished\n");

    });
  }

  bool is_black () const noexcept {
    return is_black_;
  }

  bool is_white () const noexcept {
    return !is_black_;
  }

  std::array<reversi_stone, 64> const & board () const noexcept {
    return board_;
  }

  virtual ~reversi_player () noexcept = default;

private:
  client socket_;
  reversi_client gamer_;

  std::string board_name_;

  unsigned int token_;
  bool is_black_;
  std::array<reversi_stone, 64> board_;
};

}
