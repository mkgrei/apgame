#pragma once

#include <apgame/core/socket.hpp>

namespace apgame {

struct server;

struct server_socket : public socket {

  server_socket (boost::asio::io_service & io_service)
  : socket{io_service} {
  }

  server_socket (server_socket && other) = default;

  server_socket & operator= (server_socket && other) = default;

};

}
