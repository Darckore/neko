#include "game/base_game.hpp"

namespace neko
{
  extern void on_startup(base_game& game) noexcept;
  extern void on_exit() noexcept;
}

namespace neko
{
  // Special members

  base_game::~base_game() noexcept
  {
    on_exit();
  }

  base_game::base_game() noexcept
  {
    on_startup(*this);
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
    systems::renderer().render();
  }

  // Protected members

  void base_game::quit() noexcept
  {
    core::shutdown();
  }
}