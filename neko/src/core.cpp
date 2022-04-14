#include "core/core.hpp"
#include "core/sys_registry.hpp"
#include "game/base_game.hpp"
#include "logger/logger.hpp"

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
    loop();
  }
  void core::loop() noexcept
  {
  }
  void core::quit() noexcept
  {
  }
}