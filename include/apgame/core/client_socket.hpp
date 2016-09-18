#pragma once

#include <apgame/core/socket.hpp>

#include <boost/asio.hpp>

namespace apgame {

struct client_socket : public socket {

  client_socket (boost::asio::io_service & io_service)
  : socket{io_service} {
    socket_.open(boost::asio::ip::tcp::v4());
    socket_.set_option(boost::asio::socket_base::reuse_address(true));
  }

  client_socket (client_socket && other) = default;

  client_socket & operator= (client_socket && other) = default;

  void connect (boost::asio::ip::address const & remote_address, int remote_port) {

    socket_.connect(
      boost::asio::ip::tcp::endpoint(
        remote_address,
        remote_port
      )
    );

    is_connected_ = true;
  }

private:
  int remote_port_;
  boost::asio::ip::address remote_address_;
};

}
