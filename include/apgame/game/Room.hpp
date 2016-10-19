#pragma once

#include <apgame/game/Game.hpp>

#include <mutex>
#include <string>
#include <unordered_set>

namespace apgame {

struct Room {

  Room (std::unique_ptr<Game> game)
  : game_(std::move(game)) {
    max_user_ = 32;
  }

  std::string const & getName () const noexcept {
    return name_;
  }

  void setName (std::string const & name) {
    name_ = name;
  }

  char const * getGameName () const noexcept {
    return game_name_.data();
  }

  void setGameName (std::string const & name) {
    game_name_ = name;
  }

  std::size_t getMaxUser () const noexcept {
    return max_user_;
  }

  void setMaxUser (std::size_t max_user) {
    max_user_ = max_user;
  }

/**
 *  @return
 *  error == 0: success
 *  error == 1: the room is full
 *  error == 2: the user already exists
 *  error == 3: game name is mismatched
 */
  int addUser (User * user, std::string const & game_name) {
    std::lock_guard<std::mutex> lock(mtx_);
    if (game_name_ != game_name) {
      return 3;
    }
    if (user_set_.size() >= max_user_) {
      return 1;
    }
    auto it = user_set_.find(user);
    if (it != user_set_.end()) {
      return 2;
    }
    user_set_.emplace(user);
    return 0;
  }

private:
  std::mutex mtx_;
  std::string name_;
  std::string game_name_;
  std::size_t max_user_;
  std::size_t max_game_;
  std::unique_ptr<Game> game_;
  std::unordered_set<User *> user_set_;
};

}
