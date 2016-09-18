#pragma once

#include <apgame/core/client.hpp>
#include <apgame/core/context.hpp>
#include <apgame/core/logging.hpp>

#include <apgame/game/gameid.hpp>
#include <apgame/game/game_client.hpp>
#include <apgame/game/reversi.hpp>
#include <apgame/game/reversi_player.hpp>

namespace apgame {

struct reversi_client : public game_client {

  using game_status = reversi::game_status;
  using request = reversi::request;

  reversi_client (client_option const & opt)
  : game_client{opt}
  , token_{0}{
  }

  void operator() (context & ctx) {
    ctx_ = &ctx;
  }

  template <class Handler>
  void run (Handler && handler) {
    client_.run([&] (context & ctx) {
      ctx_ = &ctx;
      if (!send_gameid_check_status(ctx, gameid::REVERSI)) {
        LOG_DEBUG("send_gameid_check_status fail.\n");
        LOG_DEBUG("stop client.\n");
        return;
      }
      init();
      LOG_DEBUG("start user handler.\n");
      handler(*this);
      LOG_DEBUG("stop client.\n");
    });
  }

  bool get_game_status (game_status & status) {
    int s;
    if (!ctx_->send(request::GET_GAME_STATUS)) {
      return false;
    }
    if (!ctx_->recieve(s)) {
      return false;
    }
    status = game_status(s);
    return true;
  }

  void put_stone (int x, int y) {

  }

private:
  context * ctx_;
  int token_;

  bool init () {
    LOG_DEBUG("start init.\n");
    if (!ctx_->send(request::INIT)) {
      return false;
    }

    bool check;
    if (!ctx_->recieve(check)) {
      LOG_DEBUG("fail.\n");
      return false;
    }

    if (!check) {
      LOG_DEBUG("fail.\n");
      return false;
    }

    LOG_DEBUG("send token = %d.\n", token_);
    if (!ctx_->send(token_)) {
      LOG_DEBUG("fail.\n");
      return false;
    }

    LOG_DEBUG("recieve token...\n");
    if (!ctx_->recieve(token_)) {
      LOG_DEBUG("fail.\n");
      return false;
    }
    LOG_DEBUG("token = %d\n", token_);

    if (token_ == 0) {
      LOG_DEBUG("fail.\n");
      return false;
    }

    if (!auth_token()) {
      LOG_DEBUG("fail.\n");
      return false;
    }

    LOG_DEBUG("end init.\n");
    return true;
  }

  bool auth_token () {
    LOG_DEBUG("start auth token.\n");
    if (!ctx_->send(token_)) {
      LOG_DEBUG("fail.\n");
      return false;
    }
    bool check;
    if (!ctx_->recieve(check)) {
      LOG_DEBUG("fail.\n");
      return false;
    }

    if (!check) {
      LOG_DEBUG("fail.\n");
      return false;
    }
    LOG_DEBUG("success.\n");
    return true;
  }
};

}
