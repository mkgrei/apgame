#pragma once

#include <apgame/socket/Socket.hpp>
#include <apgame/socket/SocketClientOption.hpp>
#include <apgame/socket/SocketContext.hpp>

#include <apgame/core/logging.hpp>

#include <boost/asio.hpp>

#include <atomic>
#include <iostream>
#include <string>

namespace apgame {

struct SocketClient {

  SocketClient (SocketClientOption const & opt)
  : io_service_{}
  , io_service_work_{io_service_}
  , opt_(opt)
  , socket_{io_service_}
  {
  }

  // SocketClient-functionality

  template <class Handler>
  void run (Handler && handler) {

    auto remote_address = opt_.remoteAddress();
    auto remote_port = opt_.remotePort();

    LOG_INFO("connect to");
    LOG_INFO("remote_address = ", remote_address);
    LOG_INFO("remote_port = ", remote_port);

    socket_.connect(remote_address, remote_port);
    LOG_INFO("connected.");

    LOG_INFO("spawn context.");
    boost::asio::spawn(io_service_,
      [&] (boost::asio::yield_context yield) {
        SocketContext ctx(socket_, io_service_, yield);
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

  boost::asio::io_service io_service_;
  boost::asio::io_service::work io_service_work_;

  SocketClientOption opt_;
  Socket socket_;
};


}
