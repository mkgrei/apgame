#pragma once

#include <apgame/game/game_api.hpp>

#include <apgame/game/reversi.hpp>
#include <apgame/game/reversi_enum.hpp>

namespace apgame {

struct reversi_api : public game_api {

  reversi_api (reversi & game, context & ctx)
  : game_{game}, ctx_{ctx} {
  }

  void run () override {
    while (true) {
      reversi_command cmd;
      if (!ctx_.recieve(cmd)) {
        LOG_DEBUG("fail to parse request.\n");
        return;
      }

      // if handle_XXX return true, keep loop.
      // if handle_XXX return false, break loop.
      switch (cmd) {
      case REVERSI_COMMAND_ADD_USER:
        if (!handle_add_user()) { return; }
        break;
      case REVERSI_COMMAND_MAKE_PLAYER:
        if (!handle_make_player()) { return; }
        break;
      case REVERSI_COMMAND_GET_GAME_STATUS:
        if (!handle_get_game_status()) { return; }
        break;
      case REVERSI_COMMAND_GET_BOARD:
        if (!handle_get_board()) { return; }
        break;
      case REVERSI_COMMAND_PUT_STONE:
        if (!handle_put_stone()) { return; }
        break;
      case REVERSI_COMMAND_CLOSE:
        if (!handle_close()) { return; }
        break;
      default:
        LOG_DEBUG("unknown command %d\n", cmd);
        return;
      }
    }
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
    LOG_DEBUG("add_user ...\n");

    unsigned int token;
    if (!ctx_.recieve(token)) {
      LOG_DEBUG("add_user ... fail.\n");
      return false;
    }
    unsigned int val;
    if (!ctx_.recieve(val)) {
      LOG_DEBUG("add_user ... fail.\n");
      return false;
    }
    LOG_DEBUG("add_user ... %08x\n", val);
     if (!ctx_.recieve(val)) {
      LOG_DEBUG("add_user ... fail.\n");
      return false;
    }
    LOG_DEBUG("add_user ... %08x\n", val);
   
    if (!game_.api_add_user(token)) {
      return false;
    }
    token_ = token;
    if (!ctx_.send(token_)) {
      LOG_DEBUG("add_user ... fail.\n");
      return false;
    }

    LOG_DEBUG("add_user ... ok ... token = %08x\n", token);
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
    if (!game_.api_get_board(board)) {
      return false;
    }
    if (!ctx_.send(board)) {
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
 *  [bool success]
 */
  bool handle_put_stone () {
    int x;
    int y;
    if (!ctx_.recieve(x)) {
      return false;
    }
    if (!ctx_.recieve(y)) {
      return false;
    }
    if (!game_.api_put_stone(token_, x, y)) {
      ctx_.send(false);
      return false;
    }
    ctx_.send(true);
    return true;
  }

/**
 *  @details
 */
  bool handle_close () {
    game_.api_close(token_);
    return true;
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
