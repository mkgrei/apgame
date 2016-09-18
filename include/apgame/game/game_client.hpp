#pragma once

#include <apgame/core/context.hpp>
#include <apgame/core/logging.hpp>

#include <apgame/game/gameid.hpp>
#include <apgame/game/reversi_client.hpp>

namespace apgame {


struct game_client {

  game_client (gameid id) noexcept
  : gameid_{id} {
  }

  void operator() (context & ctx) {
    LOG_DEBUG("start\n");

    LOG_DEBUG("send gameid...\n");
    gameid gameid_request;
    if (!ctx.send(gameid_request)) {
      LOG_DEBUG("fail\n");
      LOG_DEBUG("stop\n");
      return;
    }

    LOG_DEBUG("recieve response...\n");
    bool check;
    if (!ctx.recieve(check) || !check) {
      LOG_DEBUG("fail\n");
      LOG_DEBUG("stop\n");
      return;
    }

    LOG_DEBUG("ok. gameid = %d.\n", gameid_request);
    switch (gameid_) {
    case REVERSI:
      ctx.send(true);
      reversi_client().run(ctx); 
    default:
      LOG_DEBUG("invalid gameid");
      ctx.send(false); break;
    } 


    LOG_DEBUG("stop\n");
  }

private:
  gameid gameid_;

};

}
