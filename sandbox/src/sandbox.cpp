#include "app/sandbox.hpp"

namespace neko_game
{
  // Special members

  sandbox::~sandbox() noexcept = default;

  sandbox::sandbox() noexcept
  {
    run();
  }

  // Private members

  bool sandbox::load() noexcept
  {
    return true;
  }
  void sandbox::on_update(time_type dt) noexcept
  {
    utils::unused(dt);
  }
  void sandbox::on_render() noexcept
  {

  }

  game_ptr make_game()
  {
    return std::make_unique<sandbox>();
  }
}