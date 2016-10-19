#pragma once

#include <apgame/socket/Socket.hpp>
#include <apgame/socket/Lock.hpp>

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>

#include <mutex>

namespace apgame {

struct SocketContext {

  SocketContext (Socket & socket, boost::asio::io_service & io_service, boost::asio::yield_context & yield)
  : socket_(socket)
  , io_service_(io_service)
  , yield_(yield) {
  }

  template <class T>
  bool send (T const & data) {
    return socket_.send(data, yield_);
  }

  bool send (std::string const & data) {
    std::size_t size = data.size();
    if (!socket_.send(size, yield_)) {
      return false;
    }
    if (!socket_.send(data[0], size, yield_)) {
      return false;
    }
    return true;
  }

  bool send (std::vector<char> const & data) {
    return socket_.send(data[0], data.size(), yield_);
  }

  template <class T>
  bool send (T const & data, std::size_t size) {
    return socket_.send(data, size, yield_);
  }

  bool send (char const * data, std::size_t size) {
    return socket_.send(*data, size, yield_);
  }

  template <class T>
  bool recieve (T & data) {
    return socket_.recieve(data, yield_);
  }

  bool recieve (std::string & data, std::size_t max) {
    std::size_t size;
    if (!socket_.recieve(size, yield_)) {
      return false;
    }
    if (size > max) {
      return false;
    }
    data.resize(size);
    if (!socket_.recieve(data[0], size, yield_)) {
      return false;
    }
    return true; 
  }

  bool recieve (std::vector<char> & data, std::size_t max) {
    std::size_t size;
    if (!socket_.recieve(size, yield_)) {
      return false;
    }
    if (size > max) {
      return false;
    }
    data.resize(size);
    if (!socket_.recieve(data[0], size, yield_)) {
      return false;
    }
    return true; 
  }

  template <class T>
  bool recieve (T & data, std::size_t size) {
    return socket_.recieve(data, size, yield_);
  }

  bool recieve (char * data, std::size_t size) {
    return socket_.recieve(*data, size, yield_);
  }

  void close () {
    socket_.close();
  }

  Lock lock (std::mutex & mtx) {
    return Lock(io_service_, yield_, mtx);
  }  

protected:
  Socket & socket_;
  boost::asio::io_service & io_service_;
  boost::asio::yield_context & yield_;
};

}
