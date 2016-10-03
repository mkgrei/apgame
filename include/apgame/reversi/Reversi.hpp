#pragma once

#include <apgame/game/Game.hpp>
#include <apgame/game/enum.hpp>
#include <apgame/reversi/enum.hpp>
#include <apgame/reversi/ReversiContext.hpp>

#include <array>
#include <atomic>
#include <ctime>
#include <mutex>
#include <unordered_map>
#include <random>

namespace apgame {

struct Reversi : public Game {

  Reversi (std::string room_name)
  : Game(std::move(room_name)) {
    std::random_device device;
    random_engine_.seed(device());
  }

  GameID gameID () const noexcept override {
    return GAME_ID_REVERSI;
  }

  char const * gameName () const noexcept override {
    return "reversi";
  }

  void run (GameContext & game_context) override {

    ReversiContext ctx(game_context.socket_context, game_context.user);

    if (!handleJoin(ctx)) {
      return;
    }

    while (spin(ctx)) {}
  }

  bool spin (ReversiContext & ctx) {
    LOG_DEBUG("spin");
    ReversiCommand cmd;
    if (!ctx.socket_context.recieve(cmd)) {
      LOG_DEBUG("fail to recieve command");
      return false;
    }

    switch (cmd) {
    case REVERSI_COMMAND_GET_COLOR:
      return handleGetColor(ctx);
    case REVERSI_COMMAND_GET_STATUS:
      return handleGetStatus(ctx);
    case REVERSI_COMMAND_GET_BOARD:
      return handleGetBoard(ctx);
    case REVERSI_COMMAND_PUT_STONE:
      return handlePutStone(ctx);
    case REVERSI_COMMAND_GET_LAST_STONE:
      return handleGetLastStone(ctx);
    default:
      LOG_DEBUG("unknown command ", cmd);
      return false;
    }
  }

/**
 *  send:
 *  [int error]
 *
 *  error = 0: success
 *  error = 1: failed
 */
  bool handleJoin (ReversiContext & ctx) {
    std::lock_guard<std::mutex> lock(mtx_);
    LOG_DEBUG("handleJoin");
    if (player_name_[0].size() > 0 && player_name_[1].size() > 0) {
      LOG_ERROR("too many users");
      if (!ctx.socket_context.send(1)) {
        LOG_ERROR("failed to send error");
      }
      return false;
    }

    int i = std::uniform_int_distribution<int>(0, 1)(random_engine_);
    if (player_name_[i].size() > 0) {
      i = 1 - i;
    }
    player_name_[i] = ctx.user.name();

    if (i == 0) {
      ctx.color = REVERSI_STONE_BLACK;
    } else {
      ctx.color = REVERSI_STONE_WHITE;
    }
 
    if (player_name_[0].size() > 0 && player_name_[1].size() > 0) {
      initBoard();
      status_ = REVERSI_STATUS_BLACK_TURN;
    }

    if (!ctx.socket_context.send(0)) {
      LOG_ERROR("failed to send error");
      return false;
    }

    return true;
  }


/**
 * @details
 * send:
 * [ReversiStone color]
 */
  bool handleGetColor (ReversiContext & ctx) {
    LOG_DEBUG("handleGetColor");
    if (!ctx.socket_context.send(ctx.color)) {
      LOG_ERROR("failed to send color");
      return false;
    }
    return true;
  }

/**
 * @details
 * send:
 * [ReversiStatus status]
 */
  bool handleGetStatus (ReversiContext & ctx) {
    LOG_DEBUG("handleGetStatus");
    std::lock_guard<std::mutex> lock(mtx_);
    if (!ctx.socket_context.send(status_)) {
      LOG_ERROR("failed to send status");
      return false;
    }
    return true;
  }

/**
 *  @details
 *  send:
 *  [ReversiStone stone] * 64
 */
  bool handleGetBoard (ReversiContext & ctx) {
    LOG_DEBUG("handleGetBoard");
    std::lock_guard<std::mutex> lock(mtx_);
    if (!ctx.socket_context.send(board_)) {
      LOG_ERROR("failed to send board");
      return false;
    }
    return true;
  }

/**
 *  @details
 *  recieve:
 *  [int x][int y]
 *
 *  send:
 *  [int error]
 *
 *  error = 0: success
 *  error = 1: invalid turn
 *  error = 2: invalid put
 */
  bool handlePutStone (ReversiContext & ctx) {
    LOG_DEBUG("handlePutStone");
    std::lock_guard<std::mutex> lock(mtx_);
    int x, y;


    if (!ctx.socket_context.recieve(x)) {
      LOG_ERROR("failed to recieve x");
      return false;
    }

    if (!ctx.socket_context.recieve(y)) {
      LOG_ERROR("failed to recieve y");
      return false;
    }

    if (ctx.color == REVERSI_STONE_BLACK && status_ != REVERSI_STATUS_BLACK_TURN) {
      LOG_ERROR("failed, invalid turn");
      if (!ctx.socket_context.send(1)) {
        LOG_ERROR("failed to send error");
        return false;
      }
      return true;
    }
    if (ctx.color == REVERSI_STONE_WHITE && status_ != REVERSI_STATUS_WHITE_TURN) {
      if (!ctx.socket_context.send(1)) {
        LOG_ERROR("failed to send error");
        return false;
      }
      return true;
    }

    if (!checkPutStone(ctx.color, x, y)) {
      LOG_ERROR("invalid put");
      if (!ctx.socket_context.send(2)) {
        LOG_ERROR("failed to send error");
        return false;
      }
      return true;
    }

    if (!ctx.socket_context.send(0)) {
      LOG_ERROR("failed to send error");
      return false;
    }

    x_ = x;
    y_ = y;
    status_ = (ctx.color == REVERSI_STONE_BLACK) ? REVERSI_STATUS_WHITE_TURN : REVERSI_STATUS_BLACK_TURN;
    return true;
  }

/**
 *  @details
 *  send:
 *  [int x][int y]
 *
  */
  bool handleGetLastStone (ReversiContext & ctx) {
    LOG_DEBUG("handleGetLastStone");
    std::lock_guard<std::mutex> lock(mtx_);
    if (!ctx.socket_context.send(x_)) {
      LOG_ERROR("failed to send x");
      return false;
    }
    if (!ctx.socket_context.send(y_)) {
      LOG_ERROR("failed to send y");
      return false;
    }
    return true;
  }

private:

  mutable std::mutex mtx_;
  std::mt19937 random_engine_;

  std::array<std::string, 2> player_name_;
  std::array<ReversiStone, 64> board_;
  ReversiStatus status_;
  int x_;
  int y_;

  void initBoard () {
    for (ReversiStone & stone : board_) {
      stone = REVERSI_STONE_EMPTY;
    }

    board_[3 + 8 * 3] = REVERSI_STONE_WHITE;
    board_[4 + 8 * 4] = REVERSI_STONE_WHITE;
    board_[3 + 8 * 4] = REVERSI_STONE_BLACK;
    board_[4 + 8 * 3] = REVERSI_STONE_BLACK;

    x_ = -1;
    y_ = -1;
  }

  bool checkPutStone (int color, int x, int y) {
    if (color != REVERSI_STONE_BLACK && color != REVERSI_STONE_WHITE) {
      return false;
    }
    if (!(0 <= x && x < 8)) {
      return false;
    }
    if (!(0 <= y && y < 8)) {
      return false;
    }

    if (board_[x + 8 * y] != REVERSI_STONE_EMPTY) {
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

    std::array<ReversiStone, 64> board;
    board = board_;

    ReversiStone my_stone = ReversiStone(color);
    ReversiStone other_stone = ReversiStone(-color);

    bool flag = false;
    for (int i = 0; i < 8; ++i) {
      int kx = K[2 * i];
      int ky = K[2 * i + 1];
      int x1 = x + kx;
      int y1 = y + ky;

      if (!(0 <= x1 && x1 < 8 && 0 <= y1 && y1 < 8)) {
        continue;
      }
      if (board[x1 + 8 * y1] != other_stone) {
        continue;
      }
      x1 += kx;
      y1 += ky;

      int j = 2;
      while (0 <= x1 && x1 < 8 && 0 <= y1 && y1 < 8) {
        if (board[x1 + 8 * y1] == REVERSI_STONE_EMPTY) {
          break;
        }
        if (board[x1 + 8 * y1] == my_stone) {
          for (int l = 0; l < j; ++l) {
            int x2 = x + l * kx;
            int y2 = y + l * ky;
            board[x2 + 8 * y2] = my_stone;
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
      if (color == REVERSI_STONE_BLACK) {
        status_ = REVERSI_STATUS_WHITE_TURN;
      } else if (color == REVERSI_STONE_WHITE) {
        status_ = REVERSI_STATUS_BLACK_TURN;
      }
    }
    return flag;
  }

//   void printBoard () {
//     for (int y = 0; y < 8; ++y) {
//       for (int x = 0; x < 8; ++x) {
//         ReversiStone stone = board_[x + 8 * y];
//         if (stone == ReversiStone::EMPTY) {
//           std::cout << '.';
//         }
//         if (stone == ReversiStone::WHITE) {
//           std::cout << 'W';
//         }
//         if (stone == ReversiStone::BLACK) {
//           std::cout << 'B';
//         }
//       }
//       std::cout << std::endl;
//     }
//   }

};

}
