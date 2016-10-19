#pragma once

#include <apgame/reversi/Reversi.hpp>

#include <array>

namespace kikutake {

using namespace apgame;

struct PrimitiveMontecarloReversiPlayer {

  void operator() (int color, std::array<ReversiStone, 64> & board, int & x, int & y) {

  }



private:

  bool playOut (ReversiStatus & st

  bool checkPutStone (ReversiStatus & status, std::array<ReversiStone, 64> & board, int color, int x, int y) {
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

    static std::array<int, 16> K{
      1, 0,
      1, 1,
      0, 1,
      -1, 1,
      -1, 0,
      -1, -1,
      0, -1,
      1, -1
    };

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
      if (color == REVERSI_STONE_BLACK) {
        status = REVERSI_STATUS_WHITE_TURN;
      } else if (color == REVERSI_STONE_WHITE) {
        status = REVERSI_STATUS_BLACK_TURN;
      }
    }
    return flag;
  }

};

}
