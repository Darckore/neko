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
    using triggers    = event<trigger_evt>;

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
    using ports_info = std::bitset<maxConnections>;

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

  private:
    //
    // Clocks used to decide whether or not we should perform device detection
    //
    clock_type m_detectClk;

    //
    // Port states
    //
    ports_info m_ports{};
  };
}

#endif