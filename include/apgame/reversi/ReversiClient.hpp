#pragma once

#include <apgame/core/logging.hpp>
#include <apgame/socket/SocketContext.hpp>
#include <apgame/reversi/enum.hpp>

#include <thread>

namespace apgame {

struct ReversiClient {

  ReversiClient (SocketContext & ctx)
  : ctx_(ctx) {
  }

/**
 *  recieve:
 *  [int error]
 *
 *  error = 0: success
 *  error = 1: failed
 */
  bool callJoin (int & error) {
    LOG_DEBUG("callJoin");
    if (!ctx_.recieve(error)) {
      LOG_ERROR("failed to recieve error");
      return false;
    }
    if (error != 0) {
      LOG_ERROR("failed to join");
      return false;
    }
    return true;
  }

/**
 * @details
 * recieve:
 * [ReversiStone color]
 */
  bool callGetColor (ReversiStone & color) {
    LOG_DEBUG("callGetColor");
    if (!ctx_.send(REVERSI_COMMAND_GET_COLOR)){
      LOG_ERROR("failed to send command");
      return false;
    }
    if (!ctx_.recieve(color)) {
      LOG_ERROR("failed to recieve color");
      return false;
    }
    return true;
  }

/**
 * @details
 * send:
 * [ReversiStatus status]
 */
  bool callGetStatus (ReversiStatus & status) {
    LOG_DEBUG("callGetStatus");
    if (!ctx_.send(REVERSI_COMMAND_GET_STATUS)) {
      LOG_ERROR("failed to send command");
      return false;
    }
    if (!ctx_.recieve(status)) {
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
  bool callGetBoard (std::array<ReversiStone, 64> & board) {
    LOG_DEBUG("callGetBoard");
    if (!ctx_.send(REVERSI_COMMAND_GET_BOARD)){
      LOG_ERROR("failed to send command");
      return false;
    }
    if (!ctx_.recieve(board)) {
      LOG_ERROR("failed to send board");
      return false;
    }
    return true;
  }

/**
 *  @details
 *  send:
 *  [int x][int y]
 *
 *  recieve:
 *  [int error]
 *
 *  error = 0: success
 *  error = 1: invalid turn
 *  error = 2: invalid put
 *  error = 3: your turn is passed
 */
  bool callPutStone (int x, int y, int & error) {
    LOG_DEBUG("callPutStone x = ", x, ", y = ", y);
    if (!ctx_.send(REVERSI_COMMAND_PUT_STONE)){
      return false;
    }
    if (!ctx_.send(x)) {
      LOG_ERROR("failed to send x");
      return false;
    }
    if (!ctx_.send(y)) {
      LOG_ERROR("failed to send y");
      return false;
    }
    if (!ctx_.recieve(error)) {
      LOG_ERROR("failed to recieve error");
      return false;
    }
    return true;
  }

/**
 *  @details
 *  recieve:
 *  [int x][int y]
 *
  */
  bool callGetLastStone (int & x, int & y) {
    LOG_DEBUG("callGetLastStone");
    if (!ctx_.send(REVERSI_COMMAND_GET_LAST_STONE)){
      return false;
    }
    if (!ctx_.recieve(x)) {
      LOG_DEBUG("failed to recieve x");
      return false;
    }
    if (!ctx_.recieve(y)) {
      LOG_DEBUG("failed to recieve y");
      return false;
    }
    return true;
  }

private:
  SocketContext & ctx_;
};

}
