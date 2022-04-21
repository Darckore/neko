#pragma once
#include "core/singleton_base.hpp"

namespace neko
{
  class app_host : private singleton<app_host>
  {
  public:
    using size_type = std::int32_t;

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
    virtual bool update() noexcept = 0;
    virtual dimensions size() const noexcept = 0;
  };
}