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
  bool send (T const & data) noexcept {
    return socket_.async_send_data(data, yield_);
  }

  template <class T>
  bool recieve (T & data) noexcept {
    return socket_.async_recieve_data(data, yield_);
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
