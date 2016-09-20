#pragma once

#include <apgame/core/socket.hpp>
// #include <apgame/core/condition_variable.hpp>

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>

namespace apgame {

struct context {

  context (socket & s, boost::asio::yield_context & yield)
  : socket_{s}
  , yield_{yield} {
  }

  template <class T>
  bool send (T const & data) {
    return socket_.async_send_data(data, yield_);
  }

  bool send (std::string const & data) {
    std::size_t size = data.size();
    if (!socket_.async_send_data(size, yield_)) {
      return false;
    }
    if (!socket_.async_send_data(data[0], size, yield_)) {
      return false;
    }
    return true;
  }

  template <class T>
  bool send (T const & data, std::size_t size) {
    return socket_.async_send_data(data, size, yield_);
  }

  template <class T>
  bool recieve (T & data) {
    return socket_.async_recieve_data(data, yield_);
  }

  bool recieve (std::string & data, std::size_t max) {
    std::size_t size;
    if (!socket_.async_recieve_data(size, yield_)) {
      return false;
    }
    if (size > max) {
      return false;
    }
    data.resize(size);
    if (!socket_.async_recieve_data(data[0], size, yield_)) {
      return false;
    }
    return true; 
  }

  template <class T>
  bool recieve (T & data, std::size_t size) {
    return socket_.async_recieve_data(data, size, yield_);
  }

  void close () {
    socket_.close();
  }

//   void wait (condition_variable & cv) {
//     cv.async_wait(cv);
//   }
protected:
  socket & socket_;
  boost::asio::yield_context & yield_; 
};

}
