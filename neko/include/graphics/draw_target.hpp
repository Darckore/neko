#pragma once
#include "core/singleton_base.hpp"

namespace neko
{
  class draw_target : private singleton<draw_target>
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
    CLASS_SPECIALS_NONE_CUSTOM(draw_target);

    virtual ~draw_target() noexcept = default;

    draw_target() noexcept = default;

  public:
    virtual bool update() noexcept = 0;
    virtual dimensions size() const noexcept = 0;
  };
}