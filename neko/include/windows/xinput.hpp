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

  public:
    CLASS_SPECIALS_NONE_CUSTOM(xinput);

    ~xinput() noexcept;

  private:
    xinput() noexcept;

  public:
    virtual void update() noexcept override;
  };
}

#endif