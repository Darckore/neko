#include "windows/xinput.hpp"

#if NEK_WINDOWS

#pragma comment(lib, "xinput")

namespace neko::platform
{
  // Special members

  xinput::~xinput() noexcept = default;

  xinput::xinput() noexcept
  {
    detect_devices();
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
  }

  // Private members

  void xinput::detect_devices() noexcept
  {
    for (device_idx idx = 0; idx < maxConnections; ++idx)
    {
      XINPUT_STATE devState{};
      const auto portState = XInputGetState(idx, &devState);
      m_ports[idx] = portState == ERROR_SUCCESS;
    }

    m_detectClk();
  }
  
  void xinput::refresh_one(device_idx idx) noexcept
  {
    XINPUT_STATE devState{};
    const auto portState = XInputGetState(idx, &devState);
    if (portState != ERROR_SUCCESS)
    {
      m_ports[idx] = false;
      return;
    }


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
}

#endif