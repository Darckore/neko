//
// Platform-independent input source
//

#pragma once
#include "core/singleton_base.hpp"

namespace neko
{
  //
  // Abstract class representing platform input layer
  // Platform-dependent inherited input sources implement the details
  // Used as a handler of things like XInput support
  //
  class platform_input : public singleton<platform_input>
  {
  public:
    CLASS_SPECIALS_NONE_CUSTOM(platform_input);

    virtual ~platform_input() noexcept = default;

    platform_input() noexcept = default;

  public:
    //
    // Called every frame
    // Processes raw input from devices and generates events
    // Platform-specific input sources must override this
    //
    virtual void update() noexcept = 0;
  };
}