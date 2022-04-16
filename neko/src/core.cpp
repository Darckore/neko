#include "core/core.hpp"
#include "core/sys_registry.hpp"
#include "graphics/draw_target.hpp"
#include "game/base_game.hpp"
#include "managers/logger.hpp"

#ifdef _WIN32
  #include "windows/window.hpp"
  using surface_type = neko::platform::window;
#else
  static_assert(false, "Platform not supported");
#endif

namespace neko
{
  // Statics

  void core::startup() noexcept
  {
    if (auto&& inst = instance())
    {
      inst->run();
      return;
    }

    logger::error("Engine startup failed");
  }

  // Special members

  core::~core() noexcept
  {
    quit();
  }

  core::core(game_type& game) noexcept :
    m_game{ game }
  {
  }

  // Private members

  void core::run() noexcept
  {
    if (!systems::init_system<draw_target, surface_type>())
    {
      logger::error("Unable to init the target graphics surface");
      return;
    }

    NEK_TRACE("Starting main loop");
    loop();
  }
  void core::loop() noexcept
  {
    while (true)
    {
      if (!systems::draw_target().update())
      {
        break;
      }

      m_game.update({});
    }
  }
  void core::quit() noexcept
  {
    logger::note("Shutting down");
    systems::shutdown_system<draw_target>();
  }
}