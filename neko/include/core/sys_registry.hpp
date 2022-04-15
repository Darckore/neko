#pragma once
#include "core/singleton_base.hpp"

namespace neko
{
  namespace detail
  {
    template <typename System>
    concept engine_system = std::is_base_of_v<singleton<System>, System>;
  }

  class draw_target;

  class systems
  {
  public:
    CLASS_SPECIALS_NONE(systems);

  public:
    static decltype(auto) draw_target() noexcept
    {
      return singleton<neko::draw_target>::get();
    }

  private:
    friend class core;

    template <detail::engine_system Sys, typename Derived, typename ...Args>
      requires (std::is_base_of_v<Sys, Derived>)
    static bool init_system(Args&& ...args) noexcept
    {
      return singleton<Sys>::template create<Derived>(std::forward<Args>(args)...);
    }

    template <detail::engine_system Sys>
    static void shutdown_system() noexcept
    {
      singleton<Sys>::shutdown();
    }
  };
}