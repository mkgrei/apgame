#pragma once

#include <cstdint>
#include <sstream>
#include <thread>

namespace apgame {

std::uint64_t get_thread_id () {
  std::stringstream ss;
  ss << std::this_thread::get_id();
  return std::stoll(ss.str());
}

}
