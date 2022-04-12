#include "game/base_game.hpp"
#include "core/logger/logger.hpp"

namespace neko
{
  // Special members

  base_game::~base_game() noexcept
  {
    core::shutdown();
    logger::shutdown();
  }

  base_game::base_game() noexcept
  {
    logger::init();
    logger::note("Initialising the game");
    if (!core::create(*this))
    {
      logger::error("Engine initialisation failed");
    }
  }

  // Private members

  bool base_game::init() noexcept
  {
    if (!before_run())
    {
      logger::error("Failed to load the game");
      return false;
    }

    update(time_type{});
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