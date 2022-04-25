#pragma once
#include "core/singleton_base.hpp"
#include "managers/event.hpp"
#include "events/raw_input.hpp"

namespace neko
{
  class input final : private singleton<input>
  {
  private:
    friend class singleton<input>;

    using kb_event    = evt::kb;
    using mouse_event = evt::mouse;

  public:
    CLASS_SPECIALS_NONE_CUSTOM(input);

    ~input() noexcept;

  private:
    input() noexcept;

  private:
    void on_kb(const kb_event& e) noexcept;
    void on_mouse(const mouse_event& e) noexcept;

  private:
    event_subscriber<kb_event>    m_kb;
    event_subscriber<mouse_event> m_mouse;
  };
}