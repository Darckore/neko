#pragma once
#include "core/singleton_base.hpp"
#include "graphics/app_host.hpp"
#include "managers/config.hpp"

namespace neko
{
  namespace detail
  {
    template <typename System>
    concept engine_system = std::is_base_of_v<singleton<System>, System>;
  }

  class app_host;
  class conf_manager;

  class systems
  {
  public:
    CLASS_SPECIALS_NONE(systems);

  public:
    static decltype(auto) app_host() noexcept
    {
      return singleton<neko::app_host>::get();
    }

    static decltype(auto) config() noexcept
    {
      return singleton<neko::conf_manager>::get();
    }

    template <detail::engine_system Sys>
    static bool good() noexcept
    {
      return singleton<Sys>::good();
    }

  private:
    friend class core;

    template <detail::engine_system Sys, typename Derived, typename ...Args>
      requires (std::is_base_of_v<Sys, Derived>)
    static bool init_system(Args&& ...args) noexcept
    {
      return singleton<Sys>::template create<Derived>(std::forward<Args>(args)...);
    }

    template <detail::engine_system Sys, typename ...Args>
    static bool init_system(Args&& ...args) noexcept
    {
      return init_system<Sys, Sys>(std::forward<Args>(args)...);
    }

    template <detail::engine_system Sys>
    static void shutdown_system() noexcept
    {
      singleton<Sys>::shutdown();
    }
  };
}