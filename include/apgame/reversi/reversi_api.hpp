#pragma once

#include <apgame/game/game_api.hpp>

#include <apgame/game/reversi.hpp>
#include <apgame/game/reversi_enum.hpp>

namespace apgame {

struct reversi_api : public game_api {

  reversi_api (reversi & game, context & ctx)
  : game_(game)
  , ctx_(ctx) {
  }

  void run () override {
    bool flag = true;
    while (flag) {
      reversi_command cmd;
      if (!ctx_.recieve(cmd)) {
        LOG_DEBUG("fail to parse command\n");
        break;
      }

      // if handle_XXX return true, keep loop.
      // if handle_XXX return false, break loop.
      switch (cmd) {
      case REVERSI_COMMAND_ADD_USER:
        flag = handle_add_user();
        break;
      case REVERSI_COMMAND_MAKE_PLAYER:
        flag = handle_make_player();
        break;
      case REVERSI_COMMAND_GET_GAME_STATUS:
        flag = handle_get_game_status();
        break;
      case REVERSI_COMMAND_GET_BOARD:
        flag = handle_get_board();
        break;
      case REVERSI_COMMAND_PUT_STONE:
        flag = handle_put_stone();
        break;
      case REVERSI_COMMAND_CLOSE:
        flag = handle_close();
        break;
      default:
        LOG_DEBUG("unknown command %d\n", cmd);
        return;
      }
    }
    handle_close();
  }


private:
  reversi & game_;
  context & ctx_;
  unsigned int token_;
/**
 * @details
 * recieve:
 * [unsigned int token]
 *
 * send:
 * [unsigned int token]
 */
  bool handle_add_user () {
    LOG_DEBUG("add_user\n");

    unsigned int token;
    if (!ctx_.recieve(token)) {
      LOG_ERROR("fail to recieve token\n");
      return false;
    }

    if (!game_.api_add_user(token)) {
      token_ = 0;
    } else {
      token_ = token;
    }

    if (!ctx_.send(token_)) {
      LOG_ERROR("fail to send token = %u\n", token_);
      token_ = 0;
      return false;
    }
    LOG_DEBUG("success to add user, token = %u\n", token_);
    return true;
  }

/**
 *  @details
 *  recieve:
 *  [unsigned int token]
 *
 *  send:
 *  [bool is_black]
 */
  bool handle_make_player () {
    bool is_black;
    LOG_DEBUG("make_player ...\n");
    if (!game_.api_make_player(token_, is_black)) {
      LOG_DEBUG("make_player ... fail\n");
      return false;
    }
    if (!ctx_.send(is_black)) {
      LOG_DEBUG("make_player ... fail\n");
      return false;
    }
    LOG_DEBUG("make_player ... is_black = %s\n", is_black ? "true" : "false");
    return true;
  }

/**
 * @details
 * send:
 * [int game_status]
 */
  bool handle_get_game_status () {
//     LOG_DEBUG("get_game_status ... \n");
    reversi_status status;
    if (!game_.api_get_game_status(status)) {
      LOG_DEBUG("get_game_status ... fail\n");
      return false;
    }
    if (!ctx_.send(status)) {
      LOG_DEBUG("get_game_status ... fail\n");
      return false;
    }

//     LOG_DEBUG("get_game_status ... status = %d\n", status);
    return true;
  }

/**
 *  @details
 */
  bool handle_get_board () {
    std::array<reversi_stone, 64> board;
    LOG_DEBUG("get_board ...\n");
    if (!game_.api_get_board(board)) {
      LOG_DEBUG("get_board ... fail\n");
      return false;
    }
    if (!ctx_.send(board)) {
      LOG_DEBUG("get_board ... fail\n");
      return false;
    }
    LOG_DEBUG("get_board ... ok\n");
    return true;
  }

/**
 *  @details
 *  recieve:
 *  [int x][int y]
 *
 *  send:
 *  [bool success]
 */
  bool handle_put_stone () {
    int x = 0;
    int y = 0;
    LOG_DEBUG("put_stone\n");
    if (!ctx_.recieve(x)) {
      LOG_ERROR("put_stone: fail, recieve x\n");
      return false;
    }
    if (!ctx_.recieve(y)) {
      LOG_ERROR("put_stone: fail, recieve y\n");
      return false;
    }
    if (!game_.api_put_stone(token_, x, y)) {
      LOG_ERROR("put_stone: fail, invalid put\n");
      if (!ctx_.send(false)) {
        LOG_ERROR("put_stone: fail, send status\n");
        return false;
      }
      return true;
    }
    if (!ctx_.send(true)) {
      LOG_ERROR("put_stone: fail, send status\n");
      return false;
    }
    LOG_DEBUG("put_stone ... ok\n");
    return true;
  }

/**
 *  @details
 */
  bool handle_close () {
    game_.api_close(token_);
    return false;
  }

/**
 *  @details
 */
  bool handle_bad_request () {
    ctx_.close();
    return true;
  }

};

}
