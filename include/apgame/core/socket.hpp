#pragma once

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>

#include <iostream>
#include <atomic>

namespace apgame {

struct server;
struct client;

/**
 *  @details
 *
 *  This object is not polymorphic.
 *  DO NOT *DYNAMICALLY* CONVERT TO DERIVATIVE TYPE.
 *  In a static way, this is safe.
 */
struct socket {
  friend server;
  friend client;

  socket (boost::asio::io_service & io_service)
  : socket_{io_service} {
    is_connected_ = false;
  }

  socket (socket && other)
  : is_connected_{other.is_connected()}
  , socket_{std::move(other.socket_)} {
  }

  socket & operator= (socket && other) {
    is_connected_ = other.is_connected();
    socket_ = std::move(other.socket_);
    return *this;
  }

  template <class T>
  bool async_send_data (T const & data, boost::asio::yield_context & yield) noexcept {
    boost::system::error_code error;
    boost::asio::async_write(
      socket_,
      boost::asio::buffer(&data, sizeof(T)),
      yield[error]
    );
    return !error;
  }

  template <class T>
  bool async_recieve_data (T & data, boost::asio::yield_context & yield) noexcept {
    boost::system::error_code error;
    boost::asio::async_read(
      socket_,
      boost::asio::buffer(&data, sizeof(T)),
      boost::asio::transfer_exactly(sizeof(T)),
      yield[error]
    );
    return !error;
  }

/**
 *  @details
 *  After calling this method, upper abstraction layer can recognize socket is closed.
 *  
 */
  void close () {
    socket_.close();
    is_connected_ = false;
  }

  bool is_connected () const noexcept {
    return is_connected_;
  }

  int local_port () const noexcept {
    return socket_.local_endpoint().port();
  }

  int remote_port () const noexcept {
    return socket_.remote_endpoint().port();
  }

  std::string local_address () const {
    return socket_.local_endpoint().address().to_string();
  }

  std::string remote_address () const {
    return socket_.remote_endpoint().address().to_string();
  }
 
protected:

  std::atomic_bool is_connected_;
  boost::asio::ip::tcp::socket socket_;
};

}
