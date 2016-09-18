#pragma once

#include <apgame/core/client.hpp>
#include <apgame/core/client_option.hpp>
#include <apgame/core/logging.hpp>

#include <apgame/game/gameid.hpp>

namespace apgame {


struct game_client {

  game_client (client_option const & opt)
  : client_{opt} {
  }

protected:

  bool send_gameid_check_status (context & ctx, gameid id) {
    LOG_DEBUG("send gameid...\n");
    if (!ctx.send(id)) {
      LOG_DEBUG("fail\n");
      LOG_DEBUG("stop\n");
      return false;
    }

    LOG_DEBUG("recieve response...\n");
    bool check;
    if (!ctx.recieve(check) || !check) {
      LOG_DEBUG("fail\n");
      LOG_DEBUG("stop\n");
      return false;
    }

    LOG_DEBUG("ok. gameid = %d.\n", id);
    return true;
  }

protected:
  client client_;
};

}
