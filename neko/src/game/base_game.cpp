#include "game/base_game.hpp"

namespace neko
{
  // Special members

  base_game::~base_game() noexcept
  {
    NEK_TRACE("Exiting the game");
    core::shutdown();
    logger::shutdown();
  }

  base_game::base_game() noexcept
  {
    logger::init();
    NEK_TRACE("Logger ready");

    logger::note("Initialising the game");
    if (!core::create<core>(*this, "data"))
    {
      logger::error("Engine initialisation failed");
      return;
    }
  #ifndef NDEBUG
    else
    {
      NEK_TRACE("Done core init");
    }
  #endif
  }

  // Private members

  bool base_game::init() noexcept
  {
    if (!systems::good<conf_manager>())
    {
      NEK_TRACE("Configuration doesn't exist, shutting down");
      return false;
    }

    NEK_TRACE("Game init");
    if (!load())
    {
      logger::error("Failed to load the game");
      return false;
    }

    NEK_TRACE("Done game init");
    return true;
  }
  void base_game::update(time_type dt) noexcept
  {
    on_update(dt);
  }
  void base_game::render() noexcept
  {
    on_render();
  }

  // Protected members

  void base_game::run() noexcept
  {
    core::startup();
  }
  void base_game::quit() noexcept
  {
    core::shutdown();
  }
}