#pragma once

#include <apgame/socket/SocketContext.hpp>
#include <apgame/game/User.hpp>
#include <apgame/game/enum.hpp>
#include <apgame/reversi/enum.hpp>


namespace apgame {

struct ReversiContext {

  ReversiContext (SocketContext & socket_context, User * user)
  : socket_context(socket_context)
  , user(user) {
  }

  SocketContext & socket_context;
  User * user;
  ReversiStone color;
};

}
