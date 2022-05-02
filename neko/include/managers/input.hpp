//
// Platform-independent input layer
//

#pragma once
#include "core/singleton_base.hpp"
#include "managers/event.hpp"
#include "events/raw_input.hpp"

namespace neko
{
  //
  // Gathers input from various sources and produces events
  // according to the current control layout
  //
  class input final : private singleton<input>
  {
  private:
    friend class singleton<input>;

    //
    // Button up/down event
    //
    using btn_event      = evt::button;

    //
    // Position event such as mouse or an analog stick movement
    //
    using position_event = evt::position;

    //
    // Axis event such as mouse wheel scrolling
    //
    using axis_event     = evt::axis;

  public:
    CLASS_SPECIALS_NONE_CUSTOM(input);

    ~input() noexcept;

  private:
    input() noexcept;

  private:
    //
    // Handles button events from various sources
    //
    void on_button(const btn_event& e) noexcept;

    //
    // Handles position events from various sources
    //
    void on_position(const position_event& e) noexcept;

    //
    // Handles axis events from various sources
    //
    void on_axis(const axis_event& e) noexcept;

  private:
    //
    // Button event subscriber
    //
    event_subscriber<btn_event>      m_btnSub;

    //
    // Position event subscriber
    //
    event_subscriber<position_event> m_posSub;

    //
    // Axis event subscriber
    //
    event_subscriber<axis_event>     m_axisSub;
  };
}