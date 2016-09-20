#pragma once

#include <apgame/core/client.hpp>
#include <apgame/core/context.hpp>
#include <apgame/core/logging.hpp>

#include <apgame/game/game_enum.hpp>
#include <apgame/game/game_client.hpp>
// #include <apgame/game/reversi.hpp>
#include <apgame/game/reversi_enum.hpp>
#include <apgame/game/reversi_api.hpp>

#include <thread>

namespace apgame {

struct reversi_client : public game_client {

  reversi_client () = default;

  bool call_add_user (unsigned int & token) {
    LOG_DEBUG("call_add_user ... token = %08x\n", token);
    if (!ctx_->send(REVERSI_COMMAND_ADD_USER)) {
      LOG_DEBUG("call_add_user ... fail\n");
      return false;
    }
    if (!ctx_->send(token)) {
      LOG_DEBUG("call_add_user ... fail\n");
      return false;
    }
    if (!ctx_->send(0x12345678u)) {
      LOG_DEBUG("call_add_user ... fail\n");
      return false;
    }
    if (!ctx_->send(0x12345678u)) {
      LOG_DEBUG("call_add_user ... fail\n");
      return false;
    }
    if (!ctx_->recieve(token)) {
      LOG_DEBUG("call_add_user ... fail\n");
      return false;
    }
 
    LOG_DEBUG("call_add_user ... ok ... token = %08x\n", token);
    return true;
  }

/**
 *  @details
 */
  bool call_make_player (bool & is_black) {
    LOG_DEBUG("call_make_player ...\n");
    if (!ctx_->send(REVERSI_COMMAND_MAKE_PLAYER)) {
      LOG_DEBUG("call_make_player ... fail\n");
      return false;
    }
    if (!ctx_->recieve(is_black)) {
      LOG_DEBUG("call_make_player ... fail\n");
      return false;
    }
    LOG_DEBUG("call_make_player ... ok ... is_black = %s\n", is_black ? "true" : "false");

    return true;
  }

/**
 * @details
 */
  bool call_get_game_status (reversi_status & status) {
    if (!ctx_->send(REVERSI_COMMAND_GET_GAME_STATUS)) {
      return false;
    }
    if (!ctx_->recieve(status)) {
      return false;
    }
    return true;
  }

/**
 * @details
 */
  bool call_get_board (std::array<reversi_stone, 64> & board) {
    if (!ctx_->send(REVERSI_COMMAND_GET_BOARD)){
      return false;
    }
    if (!ctx_->recieve(board)) {
      return false;
    }
    return true;
  }

/**
 * @details
 */
  bool call_put_stone (int x, int y, bool & success) {
    if (!ctx_->send(REVERSI_COMMAND_PUT_STONE)){
      return false;
    }
    if (!ctx_->send(x)) {
      return false;
    }
    if (!ctx_->send(y)) {
      return false;
    }
    if (!ctx_->recieve(success)) {
      return false;
    }
    if (!success) {
      return false;
    }
    return true;
  }

  bool call_command (int cmd) {
    LOG_DEBUG("command ... %d\n", cmd);
    if (!ctx_->send(cmd)) {
      LOG_DEBUG("command ... fail\n");
      return false;
    }
    LOG_DEBUG("command ... ok\n");
    return true;
  }
};

}
