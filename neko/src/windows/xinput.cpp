#include "windows/xinput.hpp"

#if NEK_WINDOWS

#pragma comment(lib, "xinput")

namespace neko::platform
{
  // Special members

  xinput::~xinput() noexcept = default;

  xinput::xinput() noexcept = default;

  // Public members

  void xinput::update() noexcept
  {

  }
}

#endif