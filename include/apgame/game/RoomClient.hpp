#pragma once

#include <apgame/game/Room.hpp>
#include <apgame/game/RoomCommand.hpp>
#include <apgame/socket/SocketContext.hpp>

#include <mutex>
#include <string>
#include <unordered_map>

namespace apgame {

struct RoomClient {

  RoomClient (SocketContext & socket_context)
  : socket_context_(socket_context) {
  }

/**
 * recieve:
 * [std::string name]
 *
 * send:
 * [int error]
 *
 * error == -2: failed
 * error == -1: communication error
 * error == 0: success
 */
  void joinRoom (std::string const & name, int & error) {
    if (!socket_context_.send(ROOM_COMMAND_JOIN_ROOM)) {
      LOG_ERROR("failed to send command");
      error = -1;
      return;
    }
    if (!socket_context_.send(name)) {
      LOG_ERROR("failed to send name");
      error = -1;
      return;
    }
    if (!socket_context_.recieve(error)) {
      LOG_ERROR("failed to recieve error");
      error = -1;
      return;
    }
  }

/**
 *  send:
 *
 *  recieve:
 *  [int error]
 *
 *  error == -1: communication error
 *  error == 0: success
 */
  void exit (int & error) {
    if (!socket_context_.send(ROOM_COMMAND_EXIT)) {
      LOG_ERROR("failed to send command");
      error = -1;
      return;
    }
    if (!socket_context_.recieve(error)) {
      LOG_ERROR("failed to recieve error");
      error = -1;
      return;
    }
  }
private:
  SocketContext & socket_context_;
};

}
