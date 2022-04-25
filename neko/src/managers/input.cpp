#include "managers/input.hpp"

namespace neko
{
  // Special members

  input::~input() noexcept = default;

  input::input() noexcept :
    NEK_EVTSUB(m_kb, on_kb),
    NEK_EVTSUB(m_mouse, on_mouse)
  {
  }

  // Private members

  void input::on_kb(const kb_event& e) noexcept
  {
    utils::unused(e);
  }

  void input::on_mouse(const mouse_event& e) noexcept
  {
    utils::unused(e);
  }
}