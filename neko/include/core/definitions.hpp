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

  [[nodiscard]] constexpr auto operator"" _nhs(const char* s, std::size_t len) noexcept
  {
    return utils::hashed_string{ s, len };
  }

  using path_type = fsys::path;

  namespace detail
  {
    template <typename T>
    concept path_initable_from = std::is_constructible_v<path_type, T>;
  }

  //
  // Root configuration path. Defined in apps
  //
  extern const path_type& root_config_path() noexcept;

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
  void on_terminate() noexcept;
}

//
// Game creation stuff
//
namespace neko
{
  class base_game;

  namespace detail
  {
    using game_ptr = pointer<base_game>;

    template <typename Game>
    concept derived_game = std::is_base_of_v<base_game, Game>;
  }

  extern detail::game_ptr make_game() noexcept;
}

//
// Registers the game class and associates it with a configuration file
// located by the specified path
//
#define NEK_REGISTER_GAME(game_class, cfg_path) namespace neko { \
  detail::game_ptr make_game() noexcept { \
    static_assert(detail::derived_game<game_class>, "Must inherit from neko::base_game");\
    return detail::game_ptr{ new (std::nothrow) game_class{} };\
  }\
  const path_type& root_config_path() noexcept { \
    static_assert(detail::path_initable_from<decltype(cfg_path)>, "Must be convertible to std::filesystem::path");\
    static path_type rootCfg { cfg_path };\
    return rootCfg;\
  }\
}