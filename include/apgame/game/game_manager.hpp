#pragma once

#include <apgame/core/context.hpp>
#include <apgame/core/logging.hpp>

#include <apgame/game/gameid.hpp>
#include <apgame/game/reversi.hpp>
#include <apgame/game/reversi_player.hpp>

namespace apgame {


struct game_manager {

  game_manager () = default;

  void operator() (context & ctx) {
    LOG_DEBUG("start\n");

    LOG_DEBUG("recieve gameid...\n");
    gameid gameid_request;
    if (!ctx.recieve(gameid_request)) {
      LOG_DEBUG("fail\n");
      LOG_DEBUG("stop\n");
      return;
    }
    LOG_DEBUG("ok. gameid = %d.\n", gameid_request);

    switch (gameid_request) {
    case REVERSI:
      ctx.send(true);
      reversi_player().run(ctx); 
    default:
      LOG_DEBUG("invalid gameid");
      ctx.send(false); break;
    } 
    LOG_DEBUG("stop\n");
  }


private:
};

}
