#include "managers/input.hpp"

namespace neko
{
  // Special members

  input::~input() noexcept = default;

  input::input() noexcept :
    NEK_EVTSUB(m_btnSub,  on_button),
    NEK_EVTSUB(m_posSub,  on_position),
    NEK_EVTSUB(m_axisSub, on_axis)
  {
  }

  // Private members

  void input::on_button(const btn_event& e) noexcept
  {
    utils::unused(e);
    NEK_TRACE("Device index {}: '0x{:02X}' ({}) key {}", e.device, e.to_char(),
               e.to_string(), (e.is_up() ? "released"sv : "pressed"sv));
  }

  void input::on_position(const position_event& e) noexcept
  {
    utils::unused(e);
    NEK_TRACE("{} {} at [{:5.4f}: {:5.4f}]", e.to_string(), e.device, e.horizontal, e.vertical);
  }

  void input::on_axis(const axis_event& e) noexcept
  {
    utils::unused(e);
    NEK_TRACE("{} {} at {:5.4f}", e.to_string(), e.device, e.delta);
  }
}