#pragma once

#include <apgame/core/client.hpp>
#include <apgame/core/context.hpp>
#include <apgame/core/logging.hpp>

#include <apgame/game/game_enum.hpp>

namespace apgame {


struct game_client {

  game_client () {
  }

  void init (context & ctx) {
    ctx_ = &ctx;
  }

  bool call_join_game (game_id id, std::string const & name) {
    LOG_INFO("join_game game_id = %d, name = %s\n", id, name.data());
    if (!ctx_->send(GAME_COMMAND_JOIN_GAME)) {
      LOG_ERROR("fail to send command\n");
      return false;
    }
    if (!ctx_->send(id)) {
      LOG_ERROR("fail to send game id\n");
      return false;
    }
    if (!ctx_->send(name)) {
      LOG_ERROR("fail to send game name\n");
      return false;
    }

    bool status;
    if (!ctx_->recieve(status)) {
      LOG_INFO("join_game ... fail\n");
      return false;
    }
    if (!status) {
      LOG_INFO("join_game ... fail\n");
      return false;
    }

    LOG_INFO("join_game ... ok\n");

    return true;
  }
protected:
  context * ctx_;
};

}
