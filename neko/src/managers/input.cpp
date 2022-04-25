#include "managers/input.hpp"

namespace neko
{
  // Special members

  input::~input() noexcept = default;

  input::input() noexcept :
    NEK_EVTSUB(m_kb, on_button),
    NEK_EVTSUB(m_mouse, on_axis)
  {
  }

  // Private members

  void input::on_button(const btn_event& e) noexcept
  {
    utils::unused(e);
    NEK_TRACE("'{}' key {}", e.code, (e.is_up() ? "released"sv : "pressed"sv));
  }

  void input::on_axis(const axis_event& e) noexcept
  {
    utils::unused(e);
  }
}