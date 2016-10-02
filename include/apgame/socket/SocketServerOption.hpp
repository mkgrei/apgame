#pragma once

#include <cassert>
#include <memory>

namespace apgame {

struct SocketServerOption {

  SocketServerOption () noexcept
  : local_port_(0)
  , local_address_("127.0.0.1")
  , num_worker_(1)
  {
  }

  int localPort () const noexcept {
    return local_port_;
  }

  SocketServerOption & localPort (int local_port) noexcept {
    local_port_ = local_port;
    return *this;
  }

  std::string localAddress () const noexcept {
    return local_address_;
  }

  SocketServerOption & localAddress (std::string const & str) {
    local_address_ = str;
    return *this;
  }

  SocketServerOption & localAddress (char const * str) {
    local_address_ = str;
    return *this;
  }

  int numWorker () const noexcept {
    return num_worker_;
  }

  SocketServerOption & numWorker (int num_worker) noexcept {
    num_worker_ = num_worker;
    return *this;
  }

  int maxConnection () const noexcept {
    return max_connection_;
  }

  SocketServerOption & maxConnection (int max_connection) noexcept {
    max_connection_ = max_connection;
    return *this;
  }

private:
  int local_port_;
  std::string local_address_;
  int num_worker_;
  int max_connection_;
};

}
