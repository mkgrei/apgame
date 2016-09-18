#pragma once

#include <apgame/core/client_option.hpp>
#include <apgame/core/client_socket.hpp>
#include <apgame/core/context.hpp>
#include <apgame/core/logging.hpp>

#include <boost/asio.hpp>

#include <atomic>
#include <iostream>
#include <string>

namespace apgame {

struct client {

  client (client_option const & opt)
  : opt_(opt)
  , io_service_{}
  , io_service_work_{io_service_}
  , socket_{io_service_}
  {
  }

  // client-functionality

  template <class Handler>
  void run (Handler && handler) {

    auto remote_address = opt_.remote_address_.to_string();
    auto remote_port = opt_.remote_port_;

    LOG_INFO("connecting ...\n");
    LOG_INFO("remote_address = %s\n", remote_address.data());
    LOG_INFO("remote_port = %d\n", remote_port);

    socket_.connect(opt_.remote_address_, opt_.remote_port_);
    LOG_INFO("connected.\n");

    LOG_INFO("spawn context.\n");
    boost::asio::spawn(io_service_,
      [&] (boost::asio::yield_context yield) {
        context ctx(socket_, yield);
        try {
          handler(ctx);
          socket_.close();
        } catch (...) {
          socket_.close();
        }
        io_service_.stop();
      }
    );
    io_service_.run();
  }

private:

  client_option opt_;
  boost::asio::io_service io_service_;
  boost::asio::io_service::work io_service_work_;
  client_socket socket_;
};


}
