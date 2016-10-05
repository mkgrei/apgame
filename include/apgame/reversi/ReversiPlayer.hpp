#include <apgame/socket/SocketClient.hpp>
#include <apgame/game/GameClient.hpp>
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
  , room_(std::move(room))
  , user_(std::move(user)) {
  }

  template <class Handler>
  void run (Handler && handler) {
    client_.run([&] (SocketContext & socket_context) {
      procGame(socket_context);
      procReversi(socket_context, handler);
    });
  }

private:
  apgame::SocketClient client_;
  std::string room_;
  std::string user_;

  void procGame (SocketContext & socket_context) {
    GameClient game_client(socket_context);
    int error;
    if (!game_client.callNegotiate(user_, error)) {
      std::printf("failed to call negotiate\n"); 
      std::exit(1);
    }
    if (error != 0) {
      std::printf("failed to negotiate\n");
      std::exit(1);
    }

    if (!game_client.callCreateRoom(room_, GameID::GAME_ID_REVERSI, error)) {
      std::printf("failed to call createRoom\n");
      std::exit(1);
    }
    if (error != 0 && error != 1) {
      std::printf("failed to create room\n");
      std::exit(1);
    }

    if (!game_client.callJoinRoom(GameID::GAME_ID_REVERSI, room_, error)) {
      std::printf("failed to call joinRoom\n");
      std::exit(1);
    }

    if (error != 0) {
      std::printf("failed to join room\n");
      std::exit(1);
    }
  };

  template <class Handler>
  void procReversi (SocketContext & socket_context, Handler && handler) {
    ReversiClient reversi_client(socket_context);

    int error;
    if (!reversi_client.callJoin(error)) {
      std::printf("failed to call join\n");
      std::exit(1);
    }
    if (error != 0) {
      std::printf("failed to join\n");
      std::exit(1);
    }

    while (true) {
      ReversiStatus status;
      if (!reversi_client.callGetStatus(status)) {
        std::printf("failed to get status\n");
        std::exit(1);
      }
      if (status != REVERSI_STATUS_BEFORE_GAME) {
        break;
      }
      std::this_thread::sleep_for(std::chrono::seconds(1));
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

      if (!reversi_client.callGetBoard(board)) {
        std::printf("failed to get board\n");
        std::exit(1);
      }

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
        std::this_thread::sleep_for(std::chrono::seconds(1));
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
};

}
