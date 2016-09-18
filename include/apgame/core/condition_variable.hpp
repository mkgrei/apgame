#pragma once

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>

namespace apgame {

struct condition_variable {

  condition_variable (boost::asio::io_service & io_service)
  : timer_{io_service} {
    timer_.expires_at(boost::posix_time::pos_infin);
  }

  template <class Handler>
  void async_wait (Handler && handler) {
    timer_.async_wait(std::forward<Handler>(handler));
  }

  void notify_one () {
    timer_.cancel_one();
  }

  void notify_all () {
    timer_.cancel();
  }

private:
  boost::asio::deadline_timer timer_;
};

}
