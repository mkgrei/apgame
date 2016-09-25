#pragma once

#include <apgame/core/context.hpp>
#include <apgame/core/logging.hpp>
#include <apgame/core/server_option.hpp>
#include <apgame/core/server_socket.hpp>
#include <apgame/core/thread.hpp>

#include <boost/asio.hpp>
#include <boost/lockfree/queue.hpp>

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

namespace apgame {

struct server {

  server (server_option const & opt)
  : opt_(opt)
  , io_service_{}
  , io_service_work_{io_service_}
  , acceptor_{
      io_service_,
      boost::asio::ip::tcp::endpoint(
        boost::asio::ip::address::from_string(opt.local_address()),
        opt.local_port()
      )
    },
    worker_(opt.num_worker()),
    is_running_{false}
  {
    LOG_INFO("server:\n");
    for (int i = 0; i < opt.max_connection(); ++i) {
      socket_.emplace_back(io_service_);
    }
    LOG_INFO("server: max_connection - %d\n", opt.max_connection());
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
    LOG_INFO("run:\n");

    auto local_address = acceptor_.local_endpoint().address().to_string();
    auto local_port = acceptor_.local_endpoint().port();
 
    LOG_INFO("run: local_address = %s\n", local_address.data());
    LOG_INFO("run: local_port = %d\n", local_port);
    LOG_INFO("run: num_worker %d\n", opt_.num_worker());

    is_running_ = true;

    // global exception handler
    try {
      for (std::thread & worker : worker_) {
        worker = std::thread([&] () {
          boost::system::error_code error;
          io_service_.run(error);
          if (error) {
            LOG_ERROR("run: worker (id = %PRlu64) has an fatal error, error message = %s\n", get_thread_id());
            LOG_ERROR("run: error = %s\n", error.message().data());
          }
        });
      }
      LOG_INFO("run: launched workers\n");
  
      register_accept_event(handler);
      LOG_INFO("run: launched acceptor socket\n");
  
      for (std::thread & worker : worker_) {
        if (worker.joinable()) {
          worker.join();
        }
      }

    } catch (std::exception & e) {
      LOG_FATAL("run: unknown error occured. message = %s\n", e.what());

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
  bool is_running () const noexcept {
    return is_running_;
  }

  // server-option

  int local_port () const noexcept {
    return opt_.local_port();
  }

  int num_worker () const noexcept {
    return opt_.num_worker();
  }

  int max_connection () const noexcept {
    return opt_.max_connection();
  }

  virtual void proc () {

  }

private:

  int max_connection_;

  server_option opt_;

  boost::asio::io_service io_service_;
  boost::asio::io_service::work io_service_work_;

  boost::asio::ip::tcp::acceptor acceptor_;
  std::vector<server_socket> socket_;

  std::vector<std::thread> worker_;
  std::atomic_bool is_running_;

/**
 *  @details
 *    thread-safety: no
 *    exception-safety: safety
 *  @pre
 *    is_running() == false;
 */
  void add_worker (int num) {
    assert(!is_running());
    try {
      worker_.reserve(num);
      for (int i = 0; i < num; ++i) {
        worker_.emplace_back();
      }
    } catch (...) {
      worker_.clear();
    }
  }

  void worker_loop () {
    while (is_running()) {
      io_service_.run();
    }
  }

  template <class Handler>
  void register_accept_event (Handler & handler) {
    for (int i = 0; i < int(socket_.size()); ++i) {
      auto & socket = socket_[i];
      if (socket.is_connected()) {
        continue;
      }

      LOG_DEBUG("disconnected socket found.\n");
      LOG_DEBUG("id = %d.\n", i);
      LOG_DEBUG("start accepting.\n", i);
      acceptor_.async_accept(
        socket.socket_,
        [&, i] (boost::system::error_code const & error) {
          if (error) {
            LOG_ERROR("acceptor has an error.\n");
            LOG_ERROR("message = %s.\n", error.message());
            return;
          }
          try { 
            on_accept(handler, socket_[i]);
          } catch (std::exception const & e) {
            LOG_ERROR("acceptor has an error.\n");
            LOG_ERROR("message = %s.\n", e.what()); 
          }
        }
      );
      return;
    }
  }

  template <class Handler>
  void on_accept (Handler & handler, server_socket & socket) {
    LOG_INFO("accept event.\n");

    auto remote_address = socket.remote_address();
    auto remote_port = socket.remote_port();

    LOG_INFO("remote_address = %s.\n", remote_address.data());
    LOG_INFO("remote_port = %d.\n", remote_port);

    socket.socket_.set_option(boost::asio::socket_base::reuse_address(true));
    socket.is_connected_ = true;

    LOG_DEBUG("spawn context.\n");
    boost::asio::spawn(io_service_, [&] (boost::asio::yield_context yield) {
      socket_context(handler, socket, yield);
    });
    LOG_DEBUG("restart accept.\n");
    register_accept_event(handler);
  }

  template <class Handler>
  void socket_context (Handler & handler, server_socket & socket, boost::asio::yield_context & yield) {
    LOG_DEBUG("start context.\n");
    context ctx(socket, yield);
    try {
      handler(ctx);
      LOG_DEBUG("finish handler.\n");
      LOG_DEBUG("close socket.\n");
      socket.close();
    } catch (std::exception const & e) {
      LOG_DEBUG("socket has an error.\n");
      LOG_DEBUG("message = %s.\n", e.what());
      LOG_DEBUG("close socket.\n");
      socket.close();
    }
    LOG_DEBUG("end context.\n");
  }
};


}
