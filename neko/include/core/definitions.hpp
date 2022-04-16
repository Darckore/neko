#pragma once
#include "managers/logger.hpp"

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
  #define NEK_TRACE(fmt, ...) logger::trace(fmt, __VA_ARGS__)
#else
  #define NEK_TRACE(fmt, ...)
#endif

#ifndef NDEBUG
namespace neko
{
  inline bool assertion(bool condition,
                        std::string_view condStr,
                        std::source_location loc = std::source_location::current())
  {
    if (condition)
      return true;

    constexpr auto fmt = "Assertion '{}' in {} failed at '{}':{}"sv;
    NEK_TRACE(fmt, condStr,
              loc.function_name(),
              loc.file_name(),
              loc.line());

    return false;
  }
}
#endif

#ifndef NDEBUG
  #define NEK_ASSERT(cond) BREAK_ON(!neko::assertion(static_cast<bool>(cond), #cond))
#else
  #define NEK_ASSERT(cond)
#endif