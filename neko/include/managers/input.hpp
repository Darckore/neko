#pragma once
#include "core/singleton_base.hpp"

namespace neko
{
  class input final : private singleton<input>
  {
  private:
    friend class singleton<input>;

  public:
    CLASS_SPECIALS_NONE_CUSTOM(input);

    ~input() noexcept;

  private:
    input() noexcept;
  };
}