#pragma once

#include <apgame/core/logging.hpp>

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>

#include <atomic>

namespace apgame {

struct Acceptor;

struct Socket {

  friend Acceptor;

  Socket (boost::asio::io_service & io_service)
  : native_socket_{io_service} {
    is_connected_ = false;
  }

  Socket (Socket && other)
  : is_connected_(other.isConnected())
  , native_socket_{std::move(other.native_socket_)} {
  }

  Socket & operator= (Socket && other) {
    is_connected_ = other.isConnected();
    native_socket_ = std::move(other.native_socket_);
    return *this;
  }

  void connect (std::string const & address, int port) {
    native_socket_.connect(
      boost::asio::ip::tcp::endpoint(
        boost::asio::ip::address::from_string(address),
        port
      )
    );
    is_connected_ = true;
  }

  template <class T>
  bool send (T const & data, boost::asio::yield_context & yield) noexcept {
    return send(data, sizeof(T), yield);
  }

  template <class T>
  bool send (T const & data, std::size_t size, boost::asio::yield_context & yield) noexcept {
    boost::system::error_code error;
    LOG_DEBUG("send ", size, " bytes");
    boost::asio::async_write(
      native_socket_,
      boost::asio::buffer(&data, size),
      boost::asio::transfer_exactly(size),
      yield[error]
    );
    return !error;
  }

  template <class T>
  bool recieve (T & data, boost::asio::yield_context & yield) noexcept {
    return recieve(data, sizeof(T), yield);
  }

  template <class T>
  bool recieve (T & data, std::size_t size, boost::asio::yield_context & yield) noexcept {
    boost::system::error_code error;
    LOG_DEBUG("recieve ", size, " bytes");
    boost::asio::async_read(
      native_socket_,
      boost::asio::buffer(&data, size),
      boost::asio::transfer_exactly(size),
      yield[error]
    );

    return !error;
  }

/**
 *  @details
 *  After calling this method, upper abstraction layer can recognize Socket is closed.
 *  
 */
  void close () {
    native_socket_.close();
    is_connected_ = false;
  }

  int localPort () const noexcept {
    return native_socket_.local_endpoint().port();
  }

  int remotePort () const noexcept {
    return native_socket_.remote_endpoint().port();
  }

  std::string localAddress () const {
    return native_socket_.local_endpoint().address().to_string();
  }

  std::string remoteAddress () const {
    return native_socket_.remote_endpoint().address().to_string();
  }

  bool isConnected () const noexcept {
    return is_connected_;
  }

  void reuseAddr (bool flag) {
    native_socket_.set_option(boost::asio::socket_base::reuse_address(flag));
  }

  bool reuseAddr () {
    boost::asio::socket_base::reuse_address option;
    native_socket_.get_option(option);
    return option.value();
  }
protected:
  std::atomic_bool is_connected_;
  boost::asio::ip::tcp::socket native_socket_;
};

}
