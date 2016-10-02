#pragma once

#include <apgame/core/logging.hpp>

#include <functional>
#include <string>

namespace apgame {

struct User {

  User (std::size_t uid, std::string name)
  : name_(std::move(name)) {
  }

  User (User && user)
  : uid_(user.uid_)
  , name_(std::move(user.name_)) {
  }

  std::size_t uid () const noexcept {
    return uid_;
  }

  std::string const & name () const noexcept {
    return name_;
  }

private:
  std::size_t uid_;
  std::string name_;
};

}

namespace std {

template <>
struct hash<apgame::User> {

  std::size_t operator() (apgame::User const & user) const noexcept {
    return hash<std::size_t>()(user.uid());
  }
};

}
