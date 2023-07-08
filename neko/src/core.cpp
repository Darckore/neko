#include "core/core.hpp"
#include "game/base_game.hpp"

#if NEK_WINDOWS
  #include "platform/windows/window.hpp"
  #include "platform/windows/xinput.hpp"
  using surface_type = neko::platform::window;
  using input_source = neko::platform::xinput;
#else
  NEK_BAD_PLATFORM
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
    if (!core::create<core>(game, root_config_path()))
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
    std::exit(-1);
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
    if (!systems::init_system<conf_manager>(cfgRoot.parent_path()))
    {
      logger::error("Unable to init configuration system");
      return;
    }

    if (!systems::config().load_file("root", cfgRoot.filename()))
    {
      logger::error("Unable to open root config file");
    }
  }

  // Private members

  bool core::init_systems() noexcept
  {
    if (!systems::init_system<app_host, surface_type>())
    {
      logger::error("Unable to init the target graphics surface");
      return false;
    }

    if (   !systems::init_system<renderer>(systems::app_host().info())
        || !systems::renderer())
    {
      logger::error("Unable to init the graphics system");
      return false;
    }
    
    if (!systems::init_system<platform_input, input_source>())
    {
      logger::error("Unable to init the input system");
      return false;
    }

    if (!systems::init_system<input>())
    {
      logger::error("Unable to init the input system");
      return false;
    }

    if (!m_game.init())
    {
      logger::error("Game init failed");
      return false;
    }

    return true;
  }

  void core::run() noexcept
  {
    if (!init_systems())
    {
      logger::error("Engine initialisation failed. Shutting down");
      return;
    }

    NEK_TRACE("Starting main loop");
    loop();
  }
  void core::loop() noexcept
  {
    while (poll_input())
    {
      m_game.update({});

      m_game.render();
    }
  }
  void core::quit() noexcept
  {
    const auto logLvl = logger::set_severity_level(logger::msg);
    logger::note("Shutting down");
    logger::set_severity_level(logLvl);

    systems::shutdown_system<input>();
    systems::shutdown_system<platform_input>();
    systems::shutdown_system<renderer>();
    systems::shutdown_system<app_host>();
    systems::shutdown_system<conf_manager>();
  }

  bool core::poll_input() noexcept
  {
    if (systems::app_host().update())
    {
      systems::platform_input().update();
      return true;
    }

    return false;
  }
}