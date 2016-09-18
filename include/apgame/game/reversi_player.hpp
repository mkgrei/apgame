#pragma once

#include <apgame/game/game_player.hpp>

#include <apgame/game/reversi.hpp>

namespace apgame {

struct reversi_player : public game_player {

  using game_status = reversi::game_status;
  using request = reversi::request;

  reversi_player ()
  : game_(new reversi) {
    token_ = 0;
  }

  reversi_player (std::shared_ptr<reversi> const & game)
  : game_{game} {
    if (!game_) {
      game_.reset(new reversi);
    }
  }

  void run (context & ctx) override {
    while (true) {
      request req;
      ctx.recieve(req);

      // if handle_XXX return true, keep loop.
      // if handle_XXX return false, break loop.
      switch (req) {
      case request::INIT:
        if (!handle_init(ctx)) { return; }
        break;
      case request::MAKE_PLAYER:
        if (!handle_make_player(ctx)) { return; }
        break;
      case request::GET_GAME_STATUS:
        if (!handle_get_game_status(ctx)) { return; }
        break;
      default:
        if (!handle_bad_request(ctx)) { return; }
        return;
      }
    }
  }

  game_status get_game_status () const noexcept {
    return game_->get_game_status();
  }


private:
  std::shared_ptr<reversi> game_;
  int token_;

  bool handle_init (context & ctx) {
    LOG_DEBUG("start init.\n");
    if (get_game_status() != game_status::BEFORE_GAME) {
      ctx.send(false);
      LOG_DEBUG("fail.\n");
      return false;
    }
    ctx.send(true);

    LOG_DEBUG("recieve token...\n");
    int token;
    if (!ctx.recieve(token)) {
      LOG_DEBUG("fail.\n");
      return false;
    }
    LOG_DEBUG("token = %d\n", token);

    if (token == 0) {
      LOG_DEBUG("generate token...\n");
      if (!game_->generate_token(token)) {
        LOG_DEBUG("fail.\n");
        ctx.send(0);
        return false;
      }
      LOG_DEBUG("token = %d\n", token);
    }

    LOG_DEBUG("send token.\n");
    if (!ctx.send(token)) {
      LOG_DEBUG("fail.\n");
      return false;
    }

    int token2;
    if (!auth_token(ctx, token2)) {
      LOG_DEBUG("fail.\n");
      return false;
    }
    if (token != token2) {
      LOG_DEBUG("fail.\n");
      return false;
    }

    token_ = token;

    return true;
  }

  bool handle_make_player (context & ctx) {
    if (!game_->make_player(token_)) {
      ctx.send(false);
      return true;
    }
    if (get_game_status() != game_status::BEFORE_GAME) {
      ctx.send(false);
      return true;
    }
    ctx.send(true);
    return true;
  }

  bool handle_get_game_status (context & ctx) {
    int status = game_->get_game_status();
    if (!ctx.send(status)) {
      return false;
    }
    return true;
  }

  bool handle_bad_request (context & ctx) {
    ctx.send(false);
    ctx.close();
    return true;
  }

  bool auth_token (context & ctx, int & token) {
    LOG_DEBUG("start auth token.\n");
    if (!ctx.recieve(token)) {
      LOG_DEBUG("fail.\n");
      return false;
    }
    if (!game_->check_token(token)) {
      LOG_DEBUG("fail.\n");
      ctx.send(false);
      return false;
    }
    ctx.send(true);
    LOG_DEBUG("success.\n");
    return true;
  }
};

}
