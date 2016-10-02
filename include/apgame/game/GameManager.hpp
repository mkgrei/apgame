#pragma once

#include <apgame/game/enum.hpp>
#include <apgame/game/User.hpp>

#include <apgame/core/logging.hpp>
#include <apgame/socket/SocketContext.hpp>
#include <apgame/reversi/Reversi.hpp>

#include <unordered_map>
#include <string>

namespace apgame {

struct GameContext {

  GameContext (SocketContext & socket, User user)
  : socket_(socket)
  , user_(std::move(user)) {
  }

  SocketContext & socket () noexcept {
    return socket_;
  }

  User & user () noexcept {
    return user_;
  }

private:
  SocketContext & socket_;
  User user_; 
};

struct GameManager {

  GameManager () {
  }

/**
 *  @details
 *
 *  recieve:
 *  [game_command cmd]
 *
 */
  void operator() (SocketContext & ctx) {
    while (spin(ctx)) {}
  }

private:

  std::mutex mtx_;
  std::unordered_map<std::string, std::unique_ptr<Game>> room_;
  std::size_t user_count;
  std::unordered_set<User> user_set_;

  bool spin (SocketContext & ctx) {
    LOG_DEBUG("spin");
    GameCommand cmd;
    if (!ctx.recieve(cmd)) {
      LOG_ERROR("failed to recieve command");
      return false;
    }
 
   switch (cmd) {
    case GAME_COMMAND_CREATE_ROOM:
      if (!handleCreateRoom(ctx)) { goto protocol_error; }
      break;
    case GAME_COMMAND_JOIN_ROOM:
        if (!handleJoinRoom(ctx)) { goto protocol_error; }
        break;
    case GAME_COMMAND_GET_ROOM_INFO:
      if (!handleGetRoomInfo(ctx)) { goto protocol_error; }
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

  bool handleNegotiate (SocketContext & ctx) {
    LOG_DEBUG("handleNegotiate");
    std::lock_guard<std::mutex> lock(mtx_);
    return false;    
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
  bool handleCreateRoom (SocketContext & ctx) {
    LOG_DEBUG("handleCreateRoom");
    std::lock_guard<std::mutex> lock(mtx_);

    std::string room_name;
    if (!ctx.recieve(room_name, 128)) {
      LOG_ERROR("failed to recieve room_name");
      return false;
    }
    LOG_DEBUG("room_name = ", room_name);

    GameID id;
    if (!ctx.recieve(id)) {
      LOG_ERROR("failed to recieve id");
    }

    if (id < 0 || GAME_ID_MAX <= id) {
      LOG_DEBUG("unknown id = ", id);
      if (!ctx.send(2)) {
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

    if (room_.count(g->room_name) > 1) {
      LOG_ERROR("room duplicated");
      if (!ctx.send(1)) {
        LOG_ERROR("failed to send response");
        return false;
      }
      return true;
    }

    if (!ctx.send(0)) {
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
 *  [GameID id][std::string room_name]
 *
 *  send:
 *  [int error]
 *
 *  error = 0: success
 *  error = 1: room_name not found
 *  error = 2: game id mismatched
 *  error = 3: rejected by game
 */
  bool handleJoinRoom (SocketContext & ctx) {
    LOG_DEBUG("handleJoinRoom");
    std::lock_guard<std::mutex> lock(mtx_);

    GameID id;
    if (!ctx.recieve(id)) {
      LOG_ERROR("failed to recieve id");
      return false;
    }
    LOG_DEBUG("recieve id = ", id);

    std::string room_name;
    if (!ctx.recieve(room_name, 128)) {
      LOG_ERROR("failed to recieve room_name");
      return false;
    }
    LOG_DEBUG("recieve room_name = ", room_name);
    auto it = room_.find(room_name);

    if (it == room_.end()) {
      LOG_ERROR("room not found");
      if (!ctx.send(1)) {
        LOG_ERROR("failed to send error");
        return false;
      }
      return true;
    }

    if (it->second->gameID() != id) {
      LOG_ERROR("game id mismatched");
      if (!ctx.send(2)) {
        LOG_ERROR("failed to send error");
        return false;
      }
      return true;
    }

    if (!ctx.send(0)) {
      LOG_ERROR("failed to send error");
      return false;
    }

    return true;
  }

/**
 *  @details
 *
 *  send:
 *  [int num_room]([std::vector<char> room_name][int game_id] * num_room)
 */
  bool handleGetRoomInfo (SocketContext & ctx) {
    LOG_DEBUG("handle_get_room_info");
    std::lock_guard<std::mutex> lock(mtx_);

    int num_room = room_.size();
    if (!ctx.send(num_room)) {
      LOG_DEBUG("failed to send num_room");
      return false;
    }
    for (auto const & p : room_) {
      if (!ctx.send(p.first)) {
        LOG_DEBUG("failed to send room_name");
        return false;
      }
      if (!ctx.send(p.second->gameID())) {
        LOG_DEBUG("failed to send game_id");
        return false;
      }
    }
    return true;
  }
};

}
