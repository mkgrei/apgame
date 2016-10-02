#pragma once

#include <apgame/socket/Socket.hpp>

#include <apgame/core/logging.hpp>

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>

namespace apgame {

struct Acceptor {

  Acceptor (boost::asio::io_service & io_service, std::string const & address, int port)
  : native_socket_{
      io_service,
      boost::asio::ip::tcp::endpoint(
        boost::asio::ip::address::from_string(address),
        port
      )
    } {
    reuseAddr(true);
  }

  Acceptor (Acceptor && other)
  : native_socket_{std::move(other.native_socket_)} {
  }

  Acceptor & operator= (Acceptor && other) {
    native_socket_ = std::move(other.native_socket_);
    return *this;
  }

  bool accept (Socket & socket, boost::asio::yield_context & yield) {
    boost::system::error_code error;
    native_socket_.async_accept(socket.native_socket_, yield[error]);
    if (!error) {
      return false;
    }
    socket.reuseAddr(true);
    return true;
  }

  void close () {
    native_socket_.close();
  }

  int localPort () const noexcept {
    return native_socket_.local_endpoint().port();
  }

  std::string localAddress () const {
    return native_socket_.local_endpoint().address().to_string();
  }

  void reuseAddr (bool flag) {
    native_socket_.set_option(boost::asio::socket_base::reuse_address(flag));
  }

  bool reuseAddr () {
    boost::asio::socket_base::reuse_address option;
    native_socket_.get_option(option);
    return option.value();
  }

private:
  boost::asio::ip::tcp::acceptor native_socket_;
};

}
