#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/system/error_code.hpp>

#include <cstdint>
#include <cstdio>
#include <cstdlib>

namespace apgame {

namespace impl {

void log () {
}

void log_ (std::int8_t const & v) {
  std::cerr << v;
}

void log_ (std::uint8_t const & v) {
  std::cerr << v;
}

void log_ (std::int16_t const & v) {
  std::cerr << v;
}

void log_ (std::uint16_t const & v) {
  std::cerr << v;
}

void log_ (std::int32_t const & v) {
  std::cerr << v;
}

void log_ (std::uint32_t const & v) {
  std::cerr << v;
}

void log_ (std::int64_t const & v) {
  std::cerr << v;
}

void log_ (std::uint64_t const & v) {
  std::cerr << v;
}

void log_ (std::string const & v) {
  std::cerr << v;
}

void log_ (char const * v) {
  std::cerr << v;
}

void log_ (boost::system::error_code const & ec) {
  std::cerr << ec.message();
}

template <class T, class ... Args>
void log (T const & t, Args const & ... args) {
  log_(t);
  impl::log(args ...);
}

}

template <class ... Args>
void log (char const * level, char const * file, int line, char const * func, Args const & ... args) {
  auto now = boost::posix_time::second_clock::local_time();

  std::cerr << '[' << level << ']';
  std::cerr << '[' << boost::posix_time::to_iso_string(now) << ']';
  std::cerr << '[' << file << ':' << line << ']';
  std::cerr << '[' << func << ']';
  std::cerr << ' ';

  impl::log(args ...);
  std::cerr << std::endl;
}


}

#define LOG_INFO(...) apgame::log("INFO", __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_DEBUG(...) apgame::log("DEBUG", __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_ERROR(...) apgame::log("ERROR", __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_FATAL(...) apgame::log("FATAL", __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
