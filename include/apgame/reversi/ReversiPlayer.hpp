#include <apgame/socket/SocketClient.hpp>
#include <apgame/game/UserClient.hpp>
#include <apgame/game/RoomClient.hpp>
#include <apgame/reversi/ReversiClient.hpp>


#include <array>
#include <chrono>
#include <iostream>
#include <string>
#include <random>

namespace apgame {

struct ReversiPlayer {

  ReversiPlayer (SocketClientOption const & opt, std::string room, std::string user)
  : client_(opt)
  , user_(std::move(user))
  , room_(std::move(room)) {
  }

  template <class Handler>
  void run (Handler && handler) {
    client_.run([&] (SocketContext & socket_context) {
      proc(socket_context, handler);
    });
  }

private:
  apgame::SocketClient client_;
  std::string user_;
  std::string room_;
  std::array<apgame::ReversiStone, 64> board_;
  int status_;

  template <class Handler>
  void proc (SocketContext & socket_context, Handler && handler) {
    UserClient user_client(socket_context);
    RoomClient room_client(socket_context);
    int error;

    user_client.joinUser(user_, error);
    if (error != 0) {
      return;
    }
    user_client.exit(error);
    if (error != 0) {
      return;
    }

    room_client.joinRoom(user_, error);
    if (error != 0) {
      return;
    }
    room_client.exit(error);
    if (error != 0) {
      return;
    }

    procReversi(socket_context, handler);
  }

  template <class Handler>
  void procReversi (SocketContext & socket_context, Handler && handler) {
    ReversiClient reversi_client(socket_context);

    while (true) {
      ReversiStatus status;
      if (!reversi_client.callGetStatus(status)) {
        std::printf("failed to get status\n");
        std::exit(1);
      }
      if (status != REVERSI_STATUS_BEFORE_GAME) {
        break;
      }
//       std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    ReversiStone color;
    if (!reversi_client.callGetColor(color)) {
      std::printf("failed to get color\n");
      std::exit(1);
    }
    std::printf("YOU ARE %s\n", color == REVERSI_STONE_BLACK ? "BLACK" : "WHITE");

    ReversiStatus status;
    int x, y;
    std::array<ReversiStone, 64> board;
    std::string message;
    while (true) {
      if (!reversi_client.callGetStatus(status)) {
        std::printf("failed to get status\n");
        std::exit(1);
      }

      status_ = status;

      if (!reversi_client.callGetBoard(board)) {
        std::printf("failed to get board\n");
        std::exit(1);
      }

      board_ = board;

      clearScreen();
      printBoard(board);
      printf("YOU ARE %s", color == REVERSI_STONE_BLACK ? "BLACK": "WHITE");
      printf("%s", message.data());

      if (status == REVERSI_STATUS_AFTER_GAME) {
        break;
      }

      if (color == REVERSI_STONE_BLACK && status == REVERSI_STATUS_BLACK_TURN) {
        if (!reversi_client.callGetLastStone(x, y)) {
          std::printf("failed to get last stone\n");
          std::exit(1);
        }
      } else if (color == REVERSI_STONE_WHITE && status == REVERSI_STATUS_WHITE_TURN) {
        if (!reversi_client.callGetLastStone(x, y)) {
          std::printf("failed to get last stone\n");
          std::exit(1);
        }
      } else if (status == REVERSI_STATUS_AFTER_GAME) {
        after_game(socket_context);
      } else {
//         std::this_thread::sleep_for(std::chrono::seconds(1));
        continue;
      }

      if (x != -1 && y != -1) {
        std::printf("enemy put stone on (%d, %d)\n", x, y);
      }
      std::printf("your turn\n");
      handler(color, board, x, y);
      std::printf("you put stone on (%d, %d)\n", x, y);
      int error;
      if (!reversi_client.callPutStone(x, y, error)) {
        std::printf("failed to get last stone\n");
        std::exit(1);
      }
      if (error == 0) {
        message = "put!\n";
      } else if (error == 3) {
        message = "passed\n";
      } else {
        message = "invalid put\n";
      }
    }

    if (!reversi_client.callGetBoard(board)) {
      std::printf("failed to get board\n");
      std::exit(1);
    }

    int count_black = 0;
    int count_white = 0;

    for (int x = 0; x < 8; ++x) {
      for (int y = 0; y < 8; ++y) {
        if (board[x + 8 * y] == REVERSI_STONE_BLACK) {
          count_black += 1;
        }
        if (board[x + 8 * y] == REVERSI_STONE_WHITE) {
          count_white += 1;
        }
      }
    }
    std::printf("game end\n");
    std::printf("BLACK: %d\n", count_black);
    std::printf("WHITE: %d\n", count_white);
  }

  void printBoard (std::array<ReversiStone, 64> & board) {
    std::printf(" 01234567\n");
    for (int y = 0; y < 8; ++y) {
      std::printf("%d", y);
      for (int x = 0; x < 8; ++x) {
        ReversiStone stone = board[x + 8 * y];
        if (stone == REVERSI_STONE_EMPTY) {
          std::printf(".");
        }
        if (stone == REVERSI_STONE_WHITE) {
          std::printf("W");
        }
        if (stone == REVERSI_STONE_BLACK) {
          std::printf("B");
        }
      }
      std::printf("\n");
    }
  }

  void clearScreen () {
    std::printf("\e[1;1H\e[2J");
  }

  void after_game (SocketContext & socket_context) {

  }

public:

  bool checkPossibleChoice (int color) {
    for (int x = 0; x < 8; ++x) {
      for (int y = 0; y < 8; ++y) {
        if (board_[x + 8 * y] != REVERSI_STONE_EMPTY) {
          continue;
        }
        if (checkPutStone(color, x, y, false)) {
          return true;
        }
      }
    }
    return false;
  }

  bool checkPutStone (int color, int x, int y, bool flip) {

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

    std::printf("%d, %d, %d\n", color, x, y);

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
      if (flip) {
          board_ = board;
        if (color == REVERSI_STONE_BLACK) {
          status_ = REVERSI_STATUS_WHITE_TURN;
        } else if (color == REVERSI_STONE_WHITE) {
          status_ = REVERSI_STATUS_BLACK_TURN;
        }
      }
    }
    return flag;
  }
};

}
