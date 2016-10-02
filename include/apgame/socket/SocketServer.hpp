#pragma once

#include <apgame/socket/Acceptor.hpp>
#include <apgame/socket/Socket.hpp>
#include <apgame/socket/SocketContext.hpp>
#include <apgame/socket/SocketServerOption.hpp>

#include <apgame/core/logging.hpp>
#include <apgame/core/thread.hpp>

#include <boost/asio.hpp>

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

namespace apgame {

struct SocketServer {

  SocketServer (SocketServerOption const & opt)
  : io_service_()
  , io_service_work_(io_service_)
  , opt_(opt)
  , acceptor_(io_service_, opt.localAddress(), opt.localPort())
  , worker_(opt.numWorker())
  , is_running_(false)
  {
    LOG_INFO("SocketServer");
    for (int i = 0; i < opt.maxConnection(); ++i) {
      socket_.emplace_back(io_service_);
    }
    LOG_INFO("constructor: max_connection = ", opt.maxConnection());
  }

  // server-functionality

/**
 *  @details
 *    thread-safety: no
 * 
 *    Internal state is valid even if multi-threads call this function.
 *    However, unintended behaviors occur probably.
 *
 *    exception-safety: no
 *
 *    Unfortunately, boost does not specify exception-safety for io_service::reset().
 */
  template <class Handler>
  void run (Handler && handler) {
    LOG_INFO("run");

    auto local_address = acceptor_.localAddress();
    auto local_port = acceptor_.localPort();
 
    LOG_INFO("local_address = ", local_address);
    LOG_INFO("local_port = ", local_port);
    LOG_INFO("num_worker = ", opt_.numWorker());

    is_running_ = true;

    // global exception handler
    try {
      for (std::thread & worker : worker_) {
        worker = std::thread([&] () {
          boost::system::error_code error;
          io_service_.run(error);
          if (error) {
            LOG_ERROR("worker (id = ", get_thread_id(), " has an fatal error, error message = ", error);
          }
        });
      }

      startAcceptLoop(handler);
  
      for (std::thread & worker : worker_) {
        if (worker.joinable()) {
          worker.join();
        }
      }

    } catch (std::exception & e) {
      LOG_FATAL("unknown error occured. message = ", e.what());

    }
  }

/**
 *  @details
 *    thread-safety: no
 *
 *    multi-thread calls may cause exception(std::system_error at worker_.join()).
 *
 *    exeption-safety: no
 *
 *    Unfortunately, boost does not specify exception-safety for io_service::stop().
 */
  void stop () {
    io_service_.stop();
    for (std::thread & worker : worker_) {
      if (worker.joinable()) {
        worker.join();
      }
    }
  }

  // server-status

/**
 *  @details
 *    thread-safety: yes
 *    exception-safety: no-throw
 */
  bool isRunning () const noexcept {
    return is_running_;
  }

  // server-option


  SocketServerOption const & option () const noexcept {
    return opt_;
  }

private:
  boost::asio::io_service io_service_;
  boost::asio::io_service::work io_service_work_;

  int max_connection_;

  SocketServerOption opt_;


  Acceptor acceptor_;
  std::vector<Socket> socket_;

  std::vector<std::thread> worker_;
  std::atomic_bool is_running_;

/**
 *  @details
 *    thread-safety: no
 *    exception-safety: safety
 *  @pre
 *    is_running() == false;
 */
  void addWorker (int num) {
    assert(!isRunning());
    try {
      worker_.reserve(num);
      for (int i = 0; i < num; ++i) {
        worker_.emplace_back();
      }
    } catch (...) {
     worker_.clear();
    }
  }

  void workerLoop () {
    while (isRunning()) {
      io_service_.run();
    }
  }

  template <class Handler>
  void startAcceptLoop (Handler && handler) {
    boost::asio::spawn(io_service_, [&] (boost::asio::yield_context yield) {
      while (acceptLoopOnce(handler, yield)) {
      }
    });
  }

  template <class Handler>
  bool acceptLoopOnce (Handler && handler, boost::asio::yield_context & yield) {
    int i = findFreeSocket();
    if (i == -1) {
      return false;
    }

    Socket & socket = socket_[i];
    LOG_INFO("accept, remote_address = ", socket.remoteAddress(), ", remote_port = ", socket.remotePort());

    if (!acceptor_.accept(socket, yield)) {
      return false;
    }

    boost::asio::spawn(io_service_, [&] (boost::asio::yield_context yield) {
      SocketContext ctx(socket, yield);
      startSocketContext(handler, ctx);
    });

    return true;
  }

  int findFreeSocket () {
    for (int i = 0; i < int(socket_.size()); ++i) {
      if (socket_[i].isConnected()) {
        return i;
      }
    }
    return -1;
  }

  template <class Handler>
  void startSocketContext (Handler & handler, SocketContext & ctx) {
    try {
      handler(ctx);
      ctx.close();
    } catch (std::exception const & e) {
      LOG_DEBUG("socket has an error");
      LOG_DEBUG("message = ", e.what());
      ctx.close();
    }
  }
};


}
