//
// XInput support
//

#pragma once

#if NEK_WINDOWS

#include "core/managers.hpp"
#include "platform/support/windows/win_includes.hpp"
#include "events/raw_input.hpp"

#include <Xinput.h>

namespace neko::platform
{
  class xinput final : public platform_input
  {
  private:
    friend class singleton<platform_input>;

    //
    // Button up/down event
    //
    using btn_evt  = evt::button;
    using btn_code = btn_evt::key_code;
    using button   = event<btn_evt>;

    //
    // Stick event
    //
    using analog_evt  = evt::position;
    using analog      = event<analog_evt>;

    //
    // Triggers
    //
    using trigger_evt = evt::axis;
    using trigger     = event<trigger_evt>;

    //
    // Index type for connected devices
    //
    using device_idx = evt::input_map::device_idx;

    //
    // Maximum number of simultaneously supported controllers
    //
    static constexpr auto maxConnections = static_cast<device_idx>(XUSER_MAX_COUNT);

    //
    // Flags indicating connected ports
    //
    using port_states = std::bitset<maxConnections>;

    //
    // States of gamepads
    //
    using pad_states = std::array<XINPUT_STATE, maxConnections>;

  public:
    CLASS_SPECIALS_NONE_CUSTOM(xinput);

    ~xinput() noexcept;

  private:
    xinput() noexcept;

  public:
    //
    // Detects devices and obtains their current state
    //
    virtual void update() noexcept override;

  private:
    //
    // Dispatches all pending events
    //
    void dispatch_events() noexcept;

    //
    // Detects connected devices
    //
    void detect_devices() noexcept;

    //
    // Gets device state by index
    //
    void refresh_one(device_idx idx) noexcept;

    //
    // Gets device states
    //
    void refresh_all() noexcept;

    //
    // Checks buttons for state change
    //
    void buttons(device_idx idx, const XINPUT_STATE& cur) noexcept;

    //
    // Checks triggers for state change
    //
    void triggers(device_idx idx, const XINPUT_STATE& cur) noexcept;

    //
    // Checks analog sticks for state change
    //
    void sticks(device_idx idx, const XINPUT_STATE& cur) noexcept;

  private:
    //
    // Clocks used to decide whether or not we should perform device detection
    //
    clock_type m_detectClk;

    //
    // Port states
    //
    port_states m_ports{};

    //
    // Previous states of gamepads
    //
    pad_states m_prev{};
  };
}

#endif