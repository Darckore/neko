#pragma once
#include "events/input_map.hpp"

namespace neko::evt
{
  struct button
  {
    enum class kind : std::uint8_t
    {
      down,
      up
    };

    using enum kind;

    using key_code = input_map::key_codes;
    using enum key_code;

    bool is_up() const noexcept
    {
      return what == up;
    }
    bool is_down() const noexcept
    {
      return what == down;
    }

    auto to_char() const noexcept
    {
      return static_cast<std::uint8_t>(code);
    }
    auto to_string() const noexcept
    {
      return input_map::to_string(code);
    }

    kind what{};
    key_code code{};
  };

  struct axis
  {
  };
}