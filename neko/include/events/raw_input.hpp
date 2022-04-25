#pragma once

namespace neko::evt
{
  struct kb
  {
    enum class kind : std::uint8_t
    {
      down,
      up
    };

    using enum kind;
    using key_code = std::uint8_t;

    bool is_up() const noexcept
    {
      return what == up;
    }
    bool is_down() const noexcept
    {
      return what == down;
    }

    kind what{};
    key_code code{};
  };

  struct mouse
  {
  };
}