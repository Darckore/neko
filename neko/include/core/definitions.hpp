//
// Common definitions used throughout the codebase
//

#pragma once

#define NEK_BAD_PLATFORM static_assert(false, "Platform not supported");

#if _WIN64
  #define NEK_WINDOWS 1
#else
#endif

namespace neko
{
  //
  // Assertion helper
  //
  bool assertion(bool, std::string_view, std::source_location = std::source_location::current());
}

#ifndef NDEBUG
  #define NEK_ASSERT(cond) BREAK_ON(!neko::assertion(static_cast<bool>(cond), #cond))
#else
  #define NEK_ASSERT(cond)
#endif

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

  //
  // A hashed string representation. Contains a string's hash code,
  // can be used as an 'id' of sorts
  //
  class hashed_string
  {
  public:
    using hash_type = utils::detail::max_int_t;

  public:
    CLASS_SPECIALS_ALL_CUSTOM(hashed_string);

    constexpr hashed_string() noexcept = default;

    constexpr hashed_string(std::string_view str) noexcept :
      m_hash{ utils::hash(str) }
    {}

    constexpr hashed_string(const char* str) noexcept :
      hashed_string{ std::string_view{ str } }
    {}

    constexpr hashed_string(const char* str, std::size_t len) noexcept :
      hashed_string{ std::string_view{ str, len } }
    {}

    constexpr hashed_string(const std::string& str) noexcept :
      hashed_string{ std::string_view{ str } }
    {}

    constexpr auto operator*() const noexcept
    {
      return m_hash;
    }

    constexpr bool operator==(const hashed_string& other) const noexcept = default;

  private:
    hash_type m_hash{};
  };

  [[nodiscard]] constexpr auto operator"" _nhs(const char* s, std::size_t len) noexcept
  {
    return hashed_string{ s, len };
  }

  using path_type = fsys::path;

  //
  // Hasher for std::filesystem::path
  //
  struct path_hasher
  {
    auto operator()(const path_type& p) const noexcept
    {
      return fsys::hash_value(p);
    }
  };
}

template <>
struct std::hash<neko::hashed_string>
{
  constexpr auto operator()(const neko::hashed_string& s) const noexcept
  {
    return *s;
  }
};

//
// Initialisation thing for lambdas calling class members.
// Use it when subscribing to events to avoid such tedious things:
// evt_sub{ this, [this](const auto& e) { on_event(e); } }
//
#define NEK_EVTSUB(sub_name, handler) sub_name{ this, [this](const auto& e) noexcept { handler(e); } }

#ifndef NDEBUG
  //
  // Posts a low-priority message to the log.
  // Can be used liberally to trace debug messages
  //
  #define NEK_TRACE(fmt, ...) neko::logger::trace(fmt, __VA_ARGS__)
#else
  #define NEK_TRACE(fmt, ...)
#endif

#ifndef NDEBUG
namespace neko
{
  inline bool assertion(bool condition,
                        std::string_view condStr,
                        std::source_location loc)
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

namespace neko
{
  //
  // std::terminate handler
  // Since neko hates exceptions (most everything is noexcept),
  // we might run into hard crashes if something goes terribly wrong
  // This one will ensure we shut down gracefully
  //
  [[noreturn]] void on_terminate() noexcept;
}