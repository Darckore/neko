#include "windows/xinput.hpp"

#if NEK_WINDOWS

#pragma comment(lib, "xinput")

namespace neko::platform
{
  // Special members

  xinput::~xinput() noexcept = default;

  xinput::xinput() noexcept
  {
    NEK_TRACE("Detecting connected XInput devices");
    detect_devices();
    NEK_TRACE("Number of connected controllers: {}", m_ports.count());
  }

  // Public members

  void xinput::update() noexcept
  {
    constexpr time_type detectInterval = 2.0f; // Detect new devices every 2 seconds
    if (m_detectClk.peek() >= detectInterval)
    {
      detect_devices();
    }

    refresh_all();

    if (!m_ports.count())
    {
      return;
    }
  }

  // Private members

  void xinput::dispatch_events() noexcept
  {
    button::dispatch();
    analog::dispatch();
    trigger::dispatch();
  }

  void xinput::detect_devices() noexcept
  {
    for (device_idx idx = 0; idx < maxConnections; ++idx)
    {
      XINPUT_STATE devState{};
      const auto portState = XInputGetState(idx, &devState) == ERROR_SUCCESS;
    #ifndef NDEBUG
      if (portState != m_ports[idx])
      {
        NEK_TRACE("Controller {} was {}", idx, (portState ? "connected"sv : "disconnected"sv));
      }
    #endif
      m_ports[idx] = portState;
    }

    m_detectClk();
  }
  
  void xinput::refresh_one(device_idx idx) noexcept
  {
    XINPUT_STATE devState{};
    const auto portState = XInputGetState(idx, &devState);
    if (portState != ERROR_SUCCESS)
    {
      NEK_TRACE("Controller {} was disconnected", idx);
      m_ports[idx] = false;
      return;
    }

    buttons(idx, devState);
    triggers(idx, devState);
    sticks(idx, devState);

    m_prev[idx] = devState;
    dispatch_events();
  }

  void xinput::refresh_all() noexcept
  {
    for (device_idx idx = 0; idx < maxConnections; ++idx)
    {
      if (m_ports[idx])
      {
        refresh_one(idx);
      }
    }
  }

  void xinput::buttons(device_idx idx, const XINPUT_STATE& cur) noexcept
  {
    using enum evt::input_map::key_codes;
    using enum evt::input_map::key_state;

    static constexpr std::array padButtons{
      PAD_A, PAD_B, PAD_X, PAD_Y,
      PAD_LS, PAD_RS, PAD_LB, PAD_RB,
      PAD_START, PAD_BACK,
      PAD_UP, PAD_DOWN, PAD_LEFT, PAD_RIGHT
    };

    static constexpr std::array xinputButtons{
      XINPUT_GAMEPAD_A, XINPUT_GAMEPAD_B, XINPUT_GAMEPAD_X, XINPUT_GAMEPAD_Y,
      XINPUT_GAMEPAD_LEFT_THUMB, XINPUT_GAMEPAD_RIGHT_THUMB,
      XINPUT_GAMEPAD_LEFT_SHOULDER, XINPUT_GAMEPAD_RIGHT_SHOULDER,
      XINPUT_GAMEPAD_START, XINPUT_GAMEPAD_BACK,
      XINPUT_GAMEPAD_DPAD_UP, XINPUT_GAMEPAD_DPAD_DOWN,
      XINPUT_GAMEPAD_DPAD_LEFT, XINPUT_GAMEPAD_DPAD_RIGHT
    };

    auto&& prevState = m_prev[idx];

    for (auto btnIdx = 0u; btnIdx < xinputButtons.size(); ++btnIdx)
    {
      const bool wasPressed = prevState.Gamepad.wButtons & xinputButtons[btnIdx];
      const bool isPressed  = cur.Gamepad.wButtons & xinputButtons[btnIdx];

      if (wasPressed == isPressed)
      {
        continue;
      }

      const auto curState = wasPressed ? RELEASED : ENGAGED;
      button::push(idx, curState, padButtons[btnIdx]);
    }
  }

  void xinput::triggers(device_idx idx, const XINPUT_STATE& cur) noexcept
  {
    auto triggerChange = [](BYTE prev, BYTE cur)
    {
      if (cur <= XINPUT_GAMEPAD_TRIGGER_THRESHOLD || prev == cur)
      {
        return coord_type{};
      }

      constexpr auto triggerMax = 255;
      return static_cast<coord_type>(cur) / triggerMax;
    };

    using enum evt::input_map::axis_src;
    const auto prevL = m_prev[idx].Gamepad.bLeftTrigger;
    const auto curL  = cur.Gamepad.bLeftTrigger;
    if (const auto ld = triggerChange(prevL, curL); !utils::eq(ld, 0.0f))
    {
      trigger::push(idx, ld, PAD_LT);
    }

    const auto prevR = m_prev[idx].Gamepad.bRightTrigger;
    const auto curR  = cur.Gamepad.bRightTrigger;
    if (const auto rd = triggerChange(prevR, curR); !utils::eq(rd, 0.0f))
    {
      trigger::push(idx, rd, PAD_RT);
    }
  }

  void xinput::sticks(device_idx idx, const XINPUT_STATE& cur) noexcept
  {
    utils::unused(idx, cur);
  }
}

#endif