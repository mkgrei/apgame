#pragma once

#include <boost/asio.hpp>

namespace apgame {

struct ConditionVariable {

  explicit
  ConditionVariable (boost::asio::io_service & io_service)
  : tiemr_(io_service) {
  }

  template <class Handler>
  void wait (Handler && handler) {
    tiemr_.async_wait(std::forward<Handler>(handler));
  }

  void no() {
    timer_.cancle_one();
  }

  void notify_all () {
    timer_.cancle();
  }

private:
  boost::asio::deadline_timer timer_;
};

}
