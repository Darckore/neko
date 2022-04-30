#include "app/sandbox.hpp"

namespace neko_game
{
  // The main thing ever

  game_ptr make_game()
  {
    return std::make_unique<sandbox>();
  }

  // Special members

  sandbox::~sandbox() noexcept = default;

  sandbox::sandbox() noexcept
  {
    NEK_TRACE("Sandbox go");
    run();
  }

  // Private members

  bool sandbox::load() noexcept
  {
    NEK_TRACE("Loading sandbox");
    return true;
  }
  void sandbox::on_update(time_type dt) noexcept
  {
    utils::unused(dt);
  }
  void sandbox::on_render() noexcept
  {

  }
}