#pragma once

#include <apgame/core/logging.hpp>
#include <apgame/socket/SocketContext.hpp>
#include <apgame/game/User.hpp>
#include <apgame/game/Room.hpp>
#include <apgame/game/Game.hpp>

#include <unordered_map>
#include <string>

namespace apgame {

struct Game;

struct GameContext {

  GameContext (SocketContext & socket_context)
  : socket_context(socket_context) {
    user = nullptr;
    room = nullptr;
  }

  SocketContext & socket_context;
  User * user;
  Room * room;
  Game * game;
};

}
