#include "core/core.hpp"
#include "managers/app_host.hpp"
#include "managers/input.hpp"
#include "game/base_game.hpp"

#if NEK_WINDOWS
  #include "windows/window.hpp"
  using surface_type = neko::platform::window;
#else
  static_assert(false, "Platform not supported");
#endif

namespace neko
{
  //
  // Performs initial setup before the core can be initialised
  //
  void on_startup(base_game& game) noexcept
  {
    logger::init();
    NEK_TRACE("Logger ready");

    const auto logLvl = logger::set_severity_level(logger::msg);
    logger::note("Initialising the game");
    logger::set_severity_level(logLvl);
    if (!core::create<core>(game, "data"))
    {
      logger::error("Engine initialisation failed");
      return;
    }

    NEK_TRACE("Done core init");
  }

  //
  // Performs final shutdown after the core has been stopped
  //
  void on_exit() noexcept
  {
    NEK_TRACE("Exiting the game");
    core::shutdown();
    logger::shutdown();
  }

  //
  // Terminate handler
  //
  void on_terminate() noexcept
  {
    bool outOfMemory = false;
    if (auto ce = std::current_exception())
    {
      try
      {
        std::rethrow_exception(ce);
      }
      catch (std::bad_alloc& e)
      {
        outOfMemory = true;
        logger::abnormal(e.what());
      }
      catch (std::exception& e)
      {
        logger::error("Uncaught exception. {}", e.what());
      }
      catch (...)
      {
        logger::error("Unknown exception");
      }
    }

    if (!outOfMemory)
    {
      logger::error("Abnormal program termination");
    }

    on_exit();
    std::_Exit(-1);
  }
}


namespace neko
{
  // Statics

  void core::startup() noexcept
  {
    if (instance)
    {
      instance->run();
      return;
    }
    
    logger::error("Engine startup failed");
  }

  // Special members

  core::~core() noexcept
  {
    quit();
  }

  core::core(game_type& game, const path_type& cfgRoot) noexcept :
    m_game{ game }
  {
    if (!systems::init_system<conf_manager>(cfgRoot))
    {
      logger::error("Unable to init configuration system");
      return;
    }

    if (!systems::config().load_file("root", "root.cfg"))
    {
      logger::error("Unable to open root config file");
    }
  }

  // Private members

  void core::run() noexcept
  {
    if (!systems::init_system<app_host, surface_type>())
    {
      logger::error("Unable to init the target graphics surface");
      return;
    }

    if (!systems::init_system<input>())
    {
      logger::error("Unable to init the input system");
      return;
    }
    
    if (!m_game.init())
    {
      logger::error("Game init failed, shutting down");
      return;
    }

    NEK_TRACE("Starting main loop");
    loop();
  }
  void core::loop() noexcept
  {
    while (true)
    {
      if (!systems::app_host().update())
      {
        break;
      }

      m_game.update({});
    }
  }
  void core::quit() noexcept
  {
    const auto logLvl = logger::set_severity_level(logger::msg);
    logger::note("Shutting down");
    logger::set_severity_level(logLvl);

    systems::shutdown_system<input>();
    systems::shutdown_system<app_host>();
    systems::shutdown_system<conf_manager>();
  }
}