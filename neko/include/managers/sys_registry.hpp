//
// Core systems manager
//

#pragma once
#include "core/singleton_base.hpp"
#include "managers/app_host.hpp"
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


  //
  // Core manager for all engine systems
  // Handles startup and shutdown, and provides access to specific systems
  //
  class systems final
  {
  public:
    CLASS_SPECIALS_NONE(systems);

  public:
    //
    // Returns a reference to the applcation host
    //
    static decltype(auto) app_host() noexcept
    {
      return singleton<neko::app_host>::get();
    }

    //
    // Returns a reference to the config manager
    //
    static decltype(auto) config() noexcept
    {
      return singleton<neko::conf_manager>::get();
    }

    //
    // Checks if a system exists
    //
    template <detail::engine_system Sys>
    static bool good() noexcept
    {
      return singleton<Sys>::good();
    }

  private:
    friend class core;

    //
    // Creates a system from a set of parameters
    // The Derived template param specifies which derived class to instantiate
    // 
    // This is handy for platform-specific stuff
    // For example, window inherits from app_host, which is an abstract class
    // We can create a window instance: init_system<app_host, window>(...)
    //
    template <detail::engine_system Sys, typename Derived, typename ...Args>
      requires (std::is_base_of_v<Sys, Derived>)
    static bool init_system(Args&& ...args) noexcept
    {
      return singleton<Sys>::template create<Derived>(std::forward<Args>(args)...);
    }

    //
    // Creates a system from a set of parameters
    //
    template <detail::engine_system Sys, typename ...Args>
    static bool init_system(Args&& ...args) noexcept
    {
      return init_system<Sys, Sys>(std::forward<Args>(args)...);
    }

    //
    // Shuts the specified system down
    //
    template <detail::engine_system Sys>
    static void shutdown_system() noexcept
    {
      singleton<Sys>::shutdown();
    }
  };
}