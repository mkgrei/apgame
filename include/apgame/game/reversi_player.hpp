#pragma once

#include <apgame/game/game_player.hpp>

#include <apgame/game/reversi.hpp>

namespace apgame {

struct reversi_player : public game_player {

  using game_status = reversi::status;
  using game_request = reversi::request;

  reversi_player ()
  : game_(new reversi) {
  }

  reversi_player (std::shared_ptr<reversi> const & game)
  : game_{game} {
    if (!game_) {
      game_.reset(new reversi);
    }
  }

  void run (context & ctx) override {
    while (true) {
      game_request req;
      ctx.recieve(req);

      // if handle_XXX return true, keep loop.
      // if handle_XXX return false, break loop.
      switch (req) {
      case game_request::Init:
        if (!handle_init(ctx)) { return; }
        break;
      case game_request::AddUser:
        if (!handle_add_user(ctx)) { return; }
        break;
      default:
        if (!handle_bad_request(ctx)) { return; }
        return;
      }
    }
  }

  reversi::status get_game_status () const noexcept {
    return game_->get_status();
  }


private:
  std::shared_ptr<reversi> game_;

  bool handleInit (context & ctx) {
    if (get_game_status() != game_status::BEFORE_START) {
      ctx.send(false);
      return false;
    }
    ctx.send(true);
    std::cout << "init" << std::endl;
    return true;
  }

  bool handle_BAD_REQUEST (context & ctx) {
    ctx.send(false);
    ctx.close();
    return true;
  }
};

}
