#pragma once

#include <boost/asio.hpp>

#include <cassert>
#include <memory>

namespace apgame {

struct SocketClientOption {

  SocketClientOption () {
  }

  int localPort () const noexcept {
    return local_port_;
  }

  SocketClientOption & localPort (int local_port) noexcept {
    local_port_ = local_port;
    return *this;
  }

  std::string const & localAddress () const noexcept {
    return local_address_str_;
  }

  SocketClientOption & localAddress (std::string const & address) noexcept {
    local_address_str_= address;
    return *this;
  }

  int remotePort () const noexcept {
    return remote_port_;
  }

  SocketClientOption & remotePort (int remote_port) noexcept {
    remote_port_ = remote_port;
    return *this;
  }

  std::string const & remoteAddress () const noexcept {
    return remote_address_str_;
  }

  SocketClientOption & remoteAddress (std::string const & address) noexcept {
    remote_address_str_ = address;
    return *this;
  }

  int maxBufferSize () const noexcept {
    return max_buffer_size_;
  }

  SocketClientOption & maxBufferSize (int max_buffer_size) noexcept {
    max_buffer_size_ = max_buffer_size;
    return *this;
  }

private:
  int local_port_;
  std::string local_address_str_;
  int remote_port_;
  std::string remote_address_str_;

  int max_buffer_size_;
};

}
