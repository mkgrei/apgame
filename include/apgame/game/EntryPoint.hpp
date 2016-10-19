#pragma once

#include <apgame/core/logging.hpp>
#include <apgame/socket/SocketContext.hpp>
#include <apgame/game/GameContext.hpp>
#include <apgame/game/RoomServer.hpp>
#include <apgame/game/UserServer.hpp>

namespace apgame {

struct EntryPoint {

  void operator() (SocketContext & socket_context) {
    LOG_INFO("entry point enter");
    GameContext game_context(socket_context);

    UserServer user_server(user_manager_, game_context);
    user_server();

    RoomServer room_server(room_manager_, game_context);
    room_server();

    if (game_context.user == nullptr || game_context.room == nullptr) {
      LOG_ERROR("failed to start game");
      return;
    }

    LOG_INFO("entry point exit");
  }

private:
  UserManager user_manager_;
  RoomManager room_manager_;
};

}
