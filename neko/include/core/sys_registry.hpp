#pragma once
#include "core/singleton_base.hpp"

namespace neko
{
  namespace detail
  {
    template <typename System>
    concept engine_system = std::is_base_of_v<singleton<System>, System>;
  }

  class systems
  {
  public:
    CLASS_SPECIALS_NONE(systems);

  private:
    friend class core;

    template <detail::engine_system Sys, typename ...Args>
    static bool init_system(Args&& ...args) noexcept
    {
      return singleton<Sys>::create(std::forward<Args>(args)...);
    }

    template <detail::engine_system Sys>
    static void shutdown_system() noexcept
    {
      singleton<Sys>::shutdown();
    }
  };
}