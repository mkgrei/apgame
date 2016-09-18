#pragma once

#include <cassert>
#include <memory>

namespace apgame {

struct server_option {

  server_option () noexcept
  : local_port_(0)
  , local_address_("127.0.0.1")
  , num_worker_(1)
  {
  }

  int local_port () const noexcept {
    return local_port_;
  }

  server_option & local_port (int local_port) noexcept {
    local_port_ = local_port;
    return *this;
  }

  std::string local_address () const noexcept {
    return local_address_;
  }

  server_option & local_address (std::string const & str) {
    local_address_ = str;
    return *this;
  }

  server_option & local_address (char const * str) {
    local_address_ = str;
    return *this;
  }

  int num_worker () const noexcept {
    return num_worker_;
  }

  server_option & num_worker (int num_worker) noexcept {
    num_worker_ = num_worker;
    return *this;
  }

  int max_connection () const noexcept {
    return max_connection_;
  }

  server_option & max_connection (int max_connection) noexcept {
    max_connection_ = max_connection;
    return *this;
  }

// /**
//  *  @details
//  *  if logger is not set, default-construct apgame::logger.
//  */
//   std::shared_ptr<apgame::logger> logger () const {
//     if (!log_) {
//       return std::shared_ptr<apgame::logger>(new apgame::logger());
//     }
//     return log_;
//   }
//
//   server_option & logger (std::shared_ptr<apgame::logger> const & log) {
//     log_ = log;
//     return *this;
//   }

  void check () const {
    assert(0 <= local_port() && local_port() <= 65535);
    assert(0 < num_worker());
    assert(0 < max_connection());
  }

private:
  int local_port_;
  std::string local_address_;
  int num_worker_;
  int max_connection_;
//   std::shared_ptr<apgame::logger> log_;
};

}
