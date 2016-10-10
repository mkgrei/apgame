#pragma once

#include <apgame/game/User.hpp>

#include <mutex>
#include <string>
#include <unordered_map>

namespace apgame {

struct UserManager {

  User * createUser (std::string const & name) {
    std::lock_guard<std::mutex> lock(mtx_);
    if (name_user_map_.count(name) > 0) {
      return nullptr;
    }
    std::unique_ptr<User> user(new User);
    User * ptr = user.get();
    user->setName(name);
    user->setUID(generateUID_(name));

    name_user_map_.emplace(user->getName(), user.get());
    uid_user_map_.emplace(user->getUID(), std::move(user));
    return ptr;
  }

  User * findUserByUID (std::size_t uid) {
    auto it = uid_user_map_.find(uid);
    if (it == uid_user_map_.end()) {
      return nullptr;
    }
    return it->second.get();
  }

  User * findUserByName (std::string const & name) {
    auto it = name_user_map_.find(name);
    if (it == name_user_map_.end()) {
      return nullptr;
    }
    return it->second;
  }
private:
  std::mutex mtx_;
  std::unordered_map<std::size_t, std::unique_ptr<User>> uid_user_map_;
  std::unordered_map<std::string, User *> name_user_map_;
  std::size_t uid_count_;

  std::size_t generateUID_ (std::string const & name) {
    return ++uid_count_;
  }
};

}
