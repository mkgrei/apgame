#pragma once

#include <apgame/game/enum.hpp>
#include <apgame/game/User.hpp>
#include <apgame/game/GameContext.hpp>

#include <apgame/core/logging.hpp>
#include <apgame/reversi/Reversi.hpp>

#include <unordered_map>
#include <string>

namespace apgame {

struct GameManager {

  GameManager () {
  }

/**
 *  @details
 *
 *  recieve:
 *  [game_command cmd]
 *
 *  return:
 *  true: run game after this function
 *  false: quit this context
 */
  void operator() (SocketContext & socket_context) {
    LOG_INFO("start GameManager");
    GameContext game_context(socket_context);
    if (!handleNegotiate(game_context)) {
      LOG_INFO("end GameManager");
      return;
    }
    while (spin(game_context)) {}
    LOG_INFO("end GameManager");
    if (!game_context.run_game) {
      return;
    }
    LOG_INFO("start Game");
    game_context.game->run(game_context);
    LOG_INFO("end Game");
  }

private:
  std::mutex mtx_;
  std::unordered_map<std::string, std::unique_ptr<Game>> room_;
  std::size_t user_count;
  std::unordered_set<User> user_set_;

  bool spin (GameContext & ctx) {
    LOG_DEBUG("spin");
    GameCommand cmd;
    if (!ctx.socket_context.recieve(cmd)) {
      LOG_ERROR("failed to recieve command");
      return false;
    }
 
   switch (cmd) {
    case GAME_COMMAND_CREATE_ROOM:
      return handleCreateRoom(ctx);
    case GAME_COMMAND_JOIN_ROOM:
      return handleJoinRoom(ctx);
    case GAME_COMMAND_GET_ROOM_INFO:
      return handleGetRoomInfo(ctx);
    default:
      LOG_ERROR("unknown command = ", cmd);
      return false;
    };
  }

/**
 *  @details
 *
 *  recieve:
 *  [std::string user_name]
 *
 *  send:
 *  [int error]
 *
 *  error = 0: success
 *  error = 1: user_name is used
 *  error = 2: invalid user_name
 */  
  bool handleNegotiate (GameContext & ctx) {
    LOG_DEBUG("handleNegotiate");

    std::string user_name;
    if (!ctx.socket_context.recieve(user_name, 128)) {
      LOG_ERROR("failed to recieve user_name");
      return false;
    }
    LOG_DEBUG("user_name = ", user_name);
    ctx.user.name(std::move(user_name));

    if (ctx.user.name().size() == 0) {
      LOG_ERROR("user_name is invalid");
      if (!ctx.socket_context.send(2)) {
        LOG_ERROR("failed to send error");
      }
      return false;
    }

    {
      std::lock_guard<std::mutex> lock(mtx_);
      if (!user_set_.count(ctx.user) == 0) {
        LOG_ERROR("user_name is used");
        if (!ctx.socket_context.send(1)) {
          LOG_ERROR("failed to send error");
        }
        return false;
      }
    }

    if (!ctx.socket_context.send(0)) {
      LOG_ERROR("failed to send error");
      return false;
    }

    return true;
  }

/**
 *  @details
 *
 *  recieve:
 *  [std::string room_name][GameID id]
 *
 *  send:
 *  [int error]
 *
 *  error = 0: success
 *  error = 1: specified room name is used
 *  error = 2: unknown game_id
 */  
  bool handleCreateRoom (GameContext & ctx) {
    LOG_DEBUG("handleCreateRoom");

    std::string room_name;
    if (!ctx.socket_context.recieve(room_name, 128)) {
      LOG_ERROR("failed to recieve room_name");
      return false;
    }
    LOG_DEBUG("room_name = ", room_name);

    GameID id;
    if (!ctx.socket_context.recieve(id)) {
      LOG_ERROR("failed to recieve id");
    }

    if (id < 0 || GAME_ID_MAX <= id) {
      LOG_DEBUG("unknown id = ", id);
      if (!ctx.socket_context.send(2)) {
        LOG_ERROR("failed to return error");
        return false;
      }
      return true;
    }
    LOG_DEBUG("id = ", gameIDStr(id));

    std::unique_ptr<Game> g;
    switch (id) {
    case GAME_ID_REVERSI:
      g.reset(new Reversi(std::move(room_name)));
      break;
    default:
      // this branch is NEVER reached
      LOG_FATAL("paranoia test failed");
      return false;
    }

    { // TODO: lock granularity
      std::lock_guard<std::mutex> lock(mtx_);
      if (room_.count(g->room_name) > 1) {
        LOG_ERROR("room duplicated");
        if (!ctx.socket_context.send(1)) {
          LOG_ERROR("failed to send response");
          return false;
        }
        return true;
      }
      room_.emplace(g->room_name, std::move(g));
    }

    if (!ctx.socket_context.send(0)) {
      LOG_ERROR("failed to send response");
      return false;
    }
    return true;
  }

/**
 *  @details
 *
 *  recieve:
 *  [GameID id][std::string room_name]
 *
 *  send:
 *  [int error]
 *
 *  error = 0: success
 *  error = 1: room_name not found
 *  error = 2: game id mismatched
 */
  bool handleJoinRoom (GameContext & ctx) {
    LOG_DEBUG("handleJoinRoom");

    GameID id;
    if (!ctx.socket_context.recieve(id)) {
      LOG_ERROR("failed to recieve id");
      return false;
    }
    LOG_DEBUG("recieve id = ", id);

    std::string room_name;
    if (!ctx.socket_context.recieve(room_name, 128)) {
      LOG_ERROR("failed to recieve room_name");
      return false;
    }
    LOG_DEBUG("recieve room_name = ", room_name);

    { // TODO: lock_guranularity
      std::lock_guard<std::mutex> lock(mtx_);
      auto it = room_.find(room_name);
      if (it == room_.end()) {
        LOG_ERROR("room not found");
        if (!ctx.socket_context.send(1)) {
          LOG_ERROR("failed to send error");
          return false;
        }
        return true;
      }
  
      if (it->second->gameID() != id) {
        LOG_ERROR("game id mismatched");
        if (!ctx.socket_context.send(2)) {
          LOG_ERROR("failed to send error");
          return false;
        }
        return true;
      }

      ctx.game = it->second.get();
    }

    if (!ctx.socket_context.send(0)) {
      LOG_ERROR("failed to send error");
      return false;
    }

    ctx.run_game = true;

    return false;
  }

/**
 *  @details
 *
 *  send:
 *  [int num_room]([std::string room_name][int game_id] * num_room)
 */
  bool handleGetRoomInfo (GameContext & ctx) {
    LOG_DEBUG("handle_get_room_info");

    { // TODO: lock guranularity
      std::lock_guard<std::mutex> lock(mtx_);
      int num_room = room_.size();
      if (!ctx.socket_context.send(num_room)) {
        LOG_DEBUG("failed to send num_room");
        return false;
      }
      for (auto const & p : room_) {
        if (!ctx.socket_context.send(p.first)) {
          LOG_DEBUG("failed to send room_name");
          return false;
        }
        if (!ctx.socket_context.send(p.second->gameID())) {
          LOG_DEBUG("failed to send game_id");
          return false;
        }
      }
    }
    return true;
  }
};

}
