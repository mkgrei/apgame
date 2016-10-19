#pragma once

#include <apgame/game/enum.hpp>
#include <apgame/game/User.hpp>
#include <apgame/game/GameContext.hpp>
#include <apgame/game/GameBuilder.hpp>
#include <apgame/game/Room.hpp>

#include <apgame/core/logging.hpp>

#include <unordered_map>
#include <string>

namespace apgame {

struct RoomManager {

  RoomManager () {
  }

  Room * createRoom (std::string const & room_name, std::string const & game_name) {
    LOG_DEBUG("createRoom");
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = name_room_map_.find(room_name);
    if (it == name_room_map_.end()) {
      return nullptr;
    }
    std::unique_ptr<Room> room(new Room(buildGame(game_name)));
    Room * ptr = room.get();
    room->setName(room_name);
    name_room_map_.emplace(room_name, std::move(room));
    return ptr;
  }

  Room * joinRoom (User * user, std::string const & room_name, std::string const & game_name) {
    LOG_DEBUG("joinRoom");
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = name_room_map_.find(room_name);
    if (it == name_room_map_.end()) {
      return nullptr;
    }
    int error = it->second->addUser(user, game_name);
    if (error != 0) {
      return nullptr;
    }
    return it->second.get();
  }

  void getRoomNameList (std::vector<std::string> & room_name_list) {
    LOG_DEBUG("getRoomNameList");
    std::lock_guard<std::mutex> lock(mtx_);
    room_name_list.resize(name_room_map_.size());
    auto first = name_room_map_.begin();
    auto last = name_room_map_.end();
    for (std::size_t i = 0; first != last; ++first, ++i) {
      room_name_list[i] = first->first;
    }
  }
private:
  std::mutex mtx_;
  std::unordered_map<std::string, std::unique_ptr<Room>> name_room_map_;

};

}
