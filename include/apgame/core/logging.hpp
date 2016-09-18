#pragma once

#include <cstdlib>

namespace apgame {

template <class ... Args>
void logging (char const * file, int line, char const * function, char const * format, Args const & ... args) {
  std::printf("[%s:%d][%s] ", file, line, function);
  std::printf(format, args ...);
}

}

#define LOG_INFO(...) apgame::logging(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_DEBUG(...) apgame::logging(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_ERROR(...) apgame::logging(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_FATAL(...) apgame::logging(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
