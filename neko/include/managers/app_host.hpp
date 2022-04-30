//
// Application host
//

#pragma once
#include "core/singleton_base.hpp"

namespace neko
{
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
    // Returns false is the application needs to close
    //
    virtual bool update() noexcept = 0;
    
    //
    // Returns host width and height
    //
    virtual dimensions size() const noexcept = 0;
  };
}