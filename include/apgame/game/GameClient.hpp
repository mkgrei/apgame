#pragma once

#include <apgame/game/enum.hpp>

#include <apgame/core/logging.hpp>
#include <apgame/socket/SocketContext.hpp>

#include <string>

namespace apgame {

struct RoomInfo {
  GameID id;
  std::string name;
};

struct GameClient {

  GameClient () {
  }

  void init (SocketContext & ctx) {
    ctx_ = &ctx;
  }

/**
 *  @details
 *
 *  send:
 *  [std::string room_name][int game_id]
 *
 *  recieve:
 *  [int error]
 *
 *  error = 0: success
 *  error = 1: specified room name is used
 *  error = 2: unknown game_id
 */  
  bool callCreateRoom (std::string const & room_name, GameID id, int & error) {
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
 *  [GameID id][std::string room_name]↲
 *↲
 *  recieve:↲
 *  [int error]↲
 *↲
 *  error = 0: success↲
 *  error = 1: room_name not found↲
 *  error = 2: game id mismatched
*/
  bool callJoinRoom (GameID id, std::string const & room_name) {
    LOG_DEBUG("callJoinRoom id = ", id, ", name = ", room_name.data());
    if (!ctx_->send(GAME_COMMAND_JOIN_ROOM)) {
      LOG_ERROR("fail to send command");
      return false;
    }
    if (!ctx_->send(id)) {
      LOG_ERROR("fail to send id");
      return false;
    }
    if (!ctx_->send(room_name)) {
      LOG_ERROR("fail to send room_name");
      return false;
    }

    int error;
    if (!ctx_->recieve(error)) {
      LOG_ERROR("failed to recieve error");
      return false;
    }
    if (error != 0) {
      LOG_INFO("failed to join, error = ", error);
      return false;
    }

    return true;
  }

/**
 *  @details
 *
 *  recieve:
 *  [int num_room]([std::string room_name][GameID id] * num_room)
 */
  bool callGetRoomInfo (std::vector<RoomInfo> & room) {
    LOG_DEBUG("callGetRoomInfo");

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
  SocketContext * ctx_;
};

}
