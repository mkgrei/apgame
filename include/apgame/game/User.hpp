#pragma once

#include <apgame/core/logging.hpp>

#include <functional>
#include <string>

namespace apgame {

struct User {

  User () {
  }

  User (User const & user)
  : name_(user.name_) {
  }

  User (User && user)
  : name_(std::move(user.name_)) {
  }

  std::string const & name () const noexcept {
    return name_;
  }

  User & name (std::string name) {
    name_ = std::move(name);
    return *this;
  }

  bool operator== (User const & other) const noexcept {
    return name_ == other.name_;
  }

  bool operator!= (User const & other) const noexcept {
    return !(*this == other);
  }

private:
  std::string name_;
};

}

namespace std {

template <>
struct hash<apgame::User> {

  std::size_t operator() (apgame::User const & user) const noexcept {
    return hash<std::string>()(user.name());
  }
};

}
