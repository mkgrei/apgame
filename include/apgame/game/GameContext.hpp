#pragma once

#include <apgame/core/logging.hpp>
#include <apgame/socket/SocketContext.hpp>
#include <apgame/game/GameContext.hpp>

#include <unordered_map>
#include <string>

namespace apgame {

struct Game;

struct GameContext {

  GameContext (SocketContext & socket_context)
  : socket_context(socket_context) {
    run_game = false;
    game = nullptr;
  }

  SocketContext & socket_context;
  User user;

  Game * game;
  bool run_game;
};

}
