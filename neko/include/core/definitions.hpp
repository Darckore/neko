#pragma once
#include "logger/logger.hpp"

namespace neko
{
  using time_type  = float;
  using coord_type = float;

  using vec2 = utils::vector<coord_type, 2>;
  using vec3 = utils::vector<coord_type, 3>;
  using mat2 = utils::matrix<coord_type, 2, 2>;
  using mat3 = utils::matrix<coord_type, 3, 3>;

  using clock_type = utils::clock<time_type>;

  template <typename T>
  using pointer = std::unique_ptr<T>;
}

#ifndef NDEBUG
  #define TRACE(fmt, ...) logger::trace(fmt, __VA_ARGS__)
#else
  #define TRACE(fmt, ...)
#endif