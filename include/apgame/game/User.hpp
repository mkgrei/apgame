#pragma once

#include <apgame/core/logging.hpp>

#include <functional>
#include <string>

namespace apgame {

struct User {

  User () {
  }

  std::string const & getName () const noexcept {
    return name_;
  }

  void setName (std::string name) {
    name_ = std::move(name);
  }

  std::size_t getUID () const noexcept {
    return uid_;
  }

  void setUID (std::size_t uid) {
    uid_ = uid;
  }

  bool operator== (User const & other) const noexcept {
    return name_ == other.name_;
  }

  bool operator!= (User const & other) const noexcept {
    return !(*this == other);
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
    return hash<std::string>()(user.getName());
  }
};

}
