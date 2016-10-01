#pragma once

#include <apgame/core/client.hpp>
#include <apgame/core/context.hpp>
#include <apgame/core/logging.hpp>

#include <apgame/game/game_enum.hpp>

#include <string>

namespace apgame {

struct room_info {
  game_id id;
  std::string name;
};

struct game_client {

  game_client () {
  }

  void init (context & ctx) {
    ctx_ = &ctx;
  }

/**
 *  @details
 *
 *  send:
 *  [vector<char> room_name][int game_id]
 *
 *  recieve:
 *  [int error]
 *
 *  error = 0: success
 *  error = 1: specified room name is used
 *  error = 2: unknown game_id
 */  
  bool call_create_room (std::string const & room_name, game_id id, int & error) {
    LOG_DEBUG("call_create_room");

   if (!ctx_->send(GAME_COMMAND_CREATE_ROOM)) {
      LOG_ERROR("failed to send command");
      return false;
    }

    if (!ctx_->send(room_name)) {
      LOG_ERROR("failed to send room_name");
      return false;
    }
    if (!ctx_->send(id)) {
      LOG_ERROR("failed to send game_id");
      return false;
    }
    if (!ctx_->recieve(error)) {
      LOG_ERROR("failed to recieve error");
      return false;
    }
    return true;
  }
/**↲
 *  @details↲
 *↲
 *  send:↲
 *  [game_id id][std::string room_name]↲
 *↲
 *  recieve:↲
 *  [int error]↲
 *↲
 *  error = 0: success↲
 *  error = 1: room_name not found↲
 *  error = 2: game id mismatched
*/
  bool call_join_room (game_id id, std::string const & name) {
    LOG_DEBUG("join_room game_id = ", id, ", name = ", name.data());
    if (!ctx_->send(GAME_COMMAND_JOIN_ROOM)) {
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
      LOG_INFO("join_room ... fail\n");
      return false;
    }
    if (!status) {
      LOG_INFO("join_room ... fail\n");
      return false;
    }

    LOG_INFO("join_room ... ok\n");

    return true;
  }

/**
 *  @details
 *
 *  recieve:
 *  [int num_room]([std::vector<char> room_name][int game_id] * num_room)
 */
  bool call_get_room_info (std::vector<room_info> & room) {
    LOG_DEBUG("call_get_room_info");

    if (!ctx_->send(GAME_COMMAND_GET_ROOM_INFO)) {
      LOG_ERROR("failed to send command");
      return false;
    }

    int num_room;
    if (!ctx_->recieve(num_room)) {
      LOG_ERROR("failed to recieve num_room");
      return false;
    }
    room.resize(num_room);

    for (int i = 0; i < num_room; ++i) {
      if (!ctx_->recieve(room[i].name, 128)) {
        LOG_ERROR("failed to recieve room_name");
        return false;
      }
      if (!ctx_->recieve(room[i].id)) {
        LOG_ERROR("failed to recieve game_id");
        return false;
      }
    }

    return true;
  }
protected:
  context * ctx_;
};

}
