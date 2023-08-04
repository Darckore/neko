#include "app/sandbox.hpp"

// The mainest thing ever
NEK_REGISTER_GAME(sanbox_thing::sandbox, "data/root.cfg")

namespace sanbox_thing
{
  // Special members

  sandbox::~sandbox() noexcept = default;

  sandbox::sandbox() noexcept
  {
    NEK_TRACE("Sandbox go");
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