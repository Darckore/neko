//
// Application host
//

#pragma once
#include "core/singleton_base.hpp"

namespace neko
{
  namespace platform
  {
    //
    // Platform-specific host information
    //
    struct host_info;
  }

  //
  // Abstract class representing the application host
  // Platform-dependent inherited windows implement the details
  // Used as the main render target, may also produce input events
  //
  class app_host : private singleton<app_host>
  {
  public:
    using size_type = std::int32_t;

    //
    // Host width and height
    //
    struct dimensions
    {
      bool operator==(const dimensions&) const noexcept = default;

      size_type width;
      size_type height;
    };

  public:
    CLASS_SPECIALS_NONE_CUSTOM(app_host);

    virtual ~app_host() noexcept = default;

    app_host() noexcept = default;

  public:
    //
    // Called every frame
    // Returns false if the application needs to close
    //
    virtual bool update() noexcept = 0;
    
    //
    // Returns host information
    //
    virtual const platform::host_info& info() const noexcept = 0;
  };
}