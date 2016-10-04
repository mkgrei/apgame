#pragma once

#include <boost/asio.hpp>

#include <mutex>

namespace apgame {

struct Lock {

  Lock (boost::asio::io_service & io_service, boost::asio::yield_context & yield, std::mutex & mtx)
  : io_service_(io_service)
  , yield_(yield)
  , mtx_(mtx) {
    lock();
  }

  ~Lock () noexcept {
    unlock();
  }

  bool try_lock () {
    return mtx_.try_lock();
  }

  void lock () {
    while (!mtx_.try_lock()) {
      io_service_.post(yield_);
    }
  }

  void unlock () {
    mtx_.unlock();
  }

private:
  boost::asio::io_service & io_service_;
  boost::asio::yield_context & yield_;
  std::mutex & mtx_;
};

}
