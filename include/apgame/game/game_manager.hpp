#pragma once

#include <apgame/core/context.hpp>
#include <apgame/core/logging.hpp>

#include <apgame/game/game_enum.hpp>
#include <apgame/game/reversi.hpp>
// #include <apgame/game/reversi_api.hpp>

#include <unordered_map>
#include <mutex>
#include <string>

namespace apgame {

struct game_manager {

  game_manager ()
  : ctx_(nullptr) {
  }

/**
 *  @details
 *
 *  recieve:
 *  [game_command cmd]
 *
 */
  void operator() (context & ctx) {
    ctx_ = &ctx;
    while (spin()) {}
  }

private:

  std::mutex mtx_;
  std::unordered_map<std::string, std::unique_ptr<game>> room_;

  context * ctx_;

  bool spin () {
    LOG_DEBUG("spin");
    game_command cmd;
    if (!ctx_->recieve(cmd)) {
      LOG_ERROR("failed to recieve command");
      return false;
    }
 
   switch (cmd) {
    case GAME_COMMAND_CREATE_ROOM:
      if (!handle_carete_game()) { goto protocol_error; }
      break;
//       case GAME_COMMAND_JOIN_GAME:
//         if (!handle_join_game()) { goto protocol_error; }
//         break;
    case GAME_COMMAND_GET_ROOM_INFO:
      if (!handle_get_room_info()) { goto protocol_error; }
      break;
    default:
      LOG_ERROR("unknown command = ", cmd);
      break;
    protocol_error:
      LOG_ERROR("protocol error");
      return false;
    };
    return true;
  }

/**
 *  @details
 *
 *  recieve:
 *  [vector<char> room_name][int game_id]
 *
 *  send:
 *  [int error]
 *
 *  error = 0: success
 *  error = 1: specified room name is used
 *  error = 2: unknown game_id
 */  
  bool handle_carete_game () {
    LOG_DEBUG("handle_create_room");

    std::string room_name;
    if (!ctx_->recieve(room_name, 128)) {
      LOG_ERROR("failed to recieve room_name");
      return false;
    }
    LOG_DEBUG("room_name = ", room_name);

    game_id gid;
    if (!ctx_->recieve(gid)) {
      LOG_ERROR("failed to recieve game_id");
      return false;
    }
    LOG_DEBUG("game_id = ", gid);

    std::unique_ptr<game> g;
    switch (gid) {
    case GAME_ID_REVERSI:
      g.reset(new reversi(std::move(room_name)));
      break;
    default:
      LOG_DEBUG("unknown gameid = ", gid);
      if (!ctx_->send(2)) { goto protocol_error; }
      return true;
    protocol_error:
      LOG_ERROR("failed to return status\n");
      return false;
    }

    std::lock_guard<std::mutex> lock(mtx_);
    if (room_.count(g->room_name) > 1) {
      LOG_ERROR("room duplicated");
      if (!ctx_->send(1)) {
        LOG_ERROR("failed to send response");
        return false;
      }
      return true;
    }

    if (!ctx_->send(0)) {
      LOG_ERROR("failed to send response");
      return false;
    }
    room_.emplace(g->room_name, std::move(g));
    return true;
  }

/**
 *  @details
 *
 *  recieve:
 *  [std::string room_name]
 *
 *  send:
 *  [int error]
 *
 *  error = 0: success
 *  error = 1: room_name not found
 */
  bool handle_join_game () {
    LOG_DEBUG("handle_join_game");
    std::string room_name;
    if (!ctx_->recieve(room_name, 128)) {
      LOG_ERROR("failed to recieve room_name");
      return false;
    }
    LOG_DEBUG("recieve room_name = ", room_name);

    if (room_.count(room_name) == 0) {
      LOG_ERROR("room not found");
      if (!ctx_->send(1)) {
        LOG_ERROR("failed to send error");
      }
      return true;
    } 
    if (!ctx_->send(0)) {
      LOG_ERROR("failed to send error");
      return false;
    }

    // call game api
  }

/**
 *  @details
 *
 *  send:
 *  [int num_room]([std::vector<char> room_name][int game_id] * num_room)
 */
  bool handle_get_room_info () {
    std::lock_guard<std::mutex> lock(mtx_);
    LOG_DEBUG("handle_get_room_info");
    int num_room = room_.size();
    if (!ctx_->send(num_room)) {
      LOG_DEBUG("failed to send num_room");
      return false;
    }
    for (auto const & p : room_) {
      if (!ctx_->send(p.first)) {
        LOG_DEBUG("failed to send room_name");
        return false;
      }
      if (!ctx_->send(p.second->get_game_id())) {
        LOG_DEBUG("failed to send game_id");
        return false;
      }
    }
    return true;
  }
};

}
