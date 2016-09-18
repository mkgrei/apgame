#pragma once

#include <apgame/core/context.hpp>
#include <apgame/core/logging.hpp>

#include <apgame/game/gameid.hpp>
#include <apgame/game/reversi.hpp>
#include <apgame/game/reversi_player.hpp>

namespace apgame {


struct reversi_client {

  reversi_client (context & ctx) noexcept
  : gameid_{id} {
  }

  bool

private:
  context & ctx_;
};

}
