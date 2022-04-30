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

    using btn_event  = evt::button;
    using position_event = evt::position;

  public:
    CLASS_SPECIALS_NONE_CUSTOM(input);

    ~input() noexcept;

  private:
    input() noexcept;

  private:
    void on_button(const btn_event& e) noexcept;
    void on_position(const position_event& e) noexcept;

  private:
    event_subscriber<btn_event>      m_kb;
    event_subscriber<position_event> m_mouse;
  };
}