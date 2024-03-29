#pragma once
#include "core/entry.hpp"

namespace sanbox_thing
{
  class sandbox : public neko::base_game
  {
  public:
    CLASS_SPECIALS_NONE_CUSTOM(sandbox);

    ~sandbox() noexcept;

    sandbox() noexcept;

  private:
    virtual bool load() noexcept override;
    virtual void on_update(time_type dt) noexcept override;
    virtual void on_render() noexcept override;
  };
}