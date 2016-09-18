#pragma once

#include <boost/asio.hpp>

#include <cassert>
#include <memory>

namespace apgame {

struct client;

struct client_option {
  friend client;

  client_option () {
    io_service_.reset(new boost::asio::io_service);
  }

  int local_port () const noexcept {
    return local_port_;
  }

  client_option & local_port (int local_port) noexcept {
    local_port_ = local_port;
    return *this;
  }

  client_option & local_address (char const * address) noexcept {
    local_address_= boost::asio::ip::address::from_string(address);
    return *this;
  }

  client_option & local_address (std::string const & address) noexcept {
    local_address_= boost::asio::ip::address::from_string(address);
    return *this;
  }

  client_option & remote_port (int remote_port) noexcept {
    remote_port_ = remote_port;
    return *this;
  }

  client_option & remote_address (char const * address) noexcept {
    remote_address_= boost::asio::ip::address::from_string(address);
    return *this;
  }

  client_option & remote_address (std::string const & address) noexcept {
    remote_address_= boost::asio::ip::address::from_string(address);
    return *this;
  }

  int max_buffer_size () const noexcept {
    return max_buffer_size_ ;
  }

  client_option & max_buffer_size (int max_buffer_size) noexcept {
    max_buffer_size_ = max_buffer_size;
    return *this;
  }

  void check () const {
    assert(0 <= local_port() && local_port() <= 65535);
    assert(0 < max_buffer_size());
  }

private:
  int local_port_;
  boost::asio::ip::address local_address_;
  int remote_port_;
  boost::asio::ip::address remote_address_;

  int max_buffer_size_;

  std::shared_ptr<boost::asio::io_service> io_service_;
};

}
