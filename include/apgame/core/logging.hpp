#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/system/error_code.hpp>

#include <cstdlib>
#include <cstdio>

namespace apgame {

namespace impl {

void log () {
}

void log (int const & v) {
  std::cout << v;
}

void log (std::size_t const & v) {
  std::cout << v;
}

void log (std::string const & v) {
  std::cout << v;
}

void log (char const * v) {
  std::cout << v;
}

void log (boost::system::error_code const & ec) {
  std::cout << ec.message();
}

template <class T, class ... Args>
void log (T const & t, Args const & ... args) {
  log(t);
  log(args ...);
}

}

template <class ... Args>
void log (char const * level, char const * file, int line, char const * func, Args const & ... args) {
  auto now = boost::posix_time::second_clock::local_time();

  std::cout << '[' << level << ']';
  std::cout << '[' << boost::posix_time::to_iso_string(now) << ']';
  std::cout << '[' << file << ':' << line << ']';
  std::cout << '[' << func << ']';
  std::cout << ' ';

  impl::log(args ...);
  std::cout << std::endl;
}


}

#define LOG_INFO(...) apgame::log("INFO", __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_DEBUG(...) apgame::log("DEBUG", __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_ERROR(...) apgame::log("ERROR", __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_FATAL(...) apgame::log("FATAL", __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
