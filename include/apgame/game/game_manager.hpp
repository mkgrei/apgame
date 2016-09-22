#pragma once

#include <apgame/core/context.hpp>
#include <apgame/core/logging.hpp>

#include <apgame/game/game_enum.hpp>
#include <apgame/game/reversi.hpp>
#include <apgame/game/reversi_api.hpp>

#include <unordered_map>

namespace apgame {

struct game_manager {

  game_manager () = default;

/**
 *  @details
 *
 *  recieve:
 *  [game_command cmd]
 *
 */
  void operator() (context & ctx) {
    ctx_ = &ctx;
    while (true) {
      game_command cmd;
      if (!ctx.recieve(cmd)) {
        LOG_ERROR("fail to recieve command\n");
        return;
      }
 
     switch (cmd) {
      case GAME_COMMAND_JOIN_GAME:
        if (!handle_join_game()) {

        }
        break;
      default:
        LOG_ERROR("unknown command = %d\n", cmd);
        break;
      };
    }
  }

private:
  std::unordered_map<std::string, std::unique_ptr<game>> game_;
  context * ctx_;

/**
 *  @details
 *
 *  recieve:
 *  [game_id id][std::string name]
 *
 *  send:
 *  [bool status]
 */
  bool handle_join_game () {
    LOG_DEBUG("handle_join_game\n");

    game_id id;
    if (!ctx_->recieve(id)) {
      LOG_ERROR("fail to recieve game_id\n");
      return false;
    }
    LOG_DEBUG("recieve id = %d\n", id);

    std::string name;

    if (!ctx_->recieve(name, 128)) {
      LOG_ERROR("fail to recieve name\n");
      return false;
    }
    LOG_DEBUG("recieve name = %s\n", name.data());


    switch (id) {
    case GAME_ID_REVERSI:
      if (!ctx_->send(true)) {
        LOG_ERROR("fail to return status\n");
      }
      join_game_reversi(name);
      return true;
    default:
      LOG_DEBUG("unknown gameid = %d\n", id);
      if (!ctx_->send(false)) {
        LOG_DEBUG("unknown gameid = %d\n", id);
      }
      return false;
    }
  }

  void join_game_reversi (std::string const & name) {
    LOG_DEBUG("join_game_reversi\n");
    if (game_.count(name) == 0) {
      LOG_DEBUG("create reversi context\n");
      game_.emplace(name, std::unique_ptr<game>(new reversi));
    }
    auto & game = *game_[name];
    reversi_api api(static_cast<reversi &>(game), *ctx_);
    api.run();
  }
};

}
