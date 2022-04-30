//
// Raw input events
//

#pragma once
#include "events/input_map.hpp"

namespace neko::evt
{
  namespace detail
  {
    //
    // A device index common to input events
    // Useless by itself
    //
    struct device_idx
    {
      using idx_type = std::uint32_t;

      //
      // Source device index
      // Allows figuring out which of same-type devices produce the input
      //
      idx_type device{};
    };
  }

  //
  // Button event
  // Indicates a binary (up/down) state of a device button
  // Generated by the platform layer
  //
  struct button : public detail::device_idx
  {
    //
    // Button state
    //
    using state = input_map::key_state;
    using enum state;

    using key_code = input_map::key_codes;
    using enum key_code;

    CLASS_SPECIALS_NODEFAULT(button);

    button(idx_type devIdx, state st, key_code c) noexcept :
      detail::device_idx{ devIdx },
      keyState{ st },
      code{ c }
    {}

    //
    // Checks if the button was released
    //
    bool is_up() const noexcept
    {
      return keyState == RELEASED;
    }

    //
    // Checks if the button was pressed
    //
    bool is_down() const noexcept
    {
      return keyState == ENGAGED;
    }

    //
    // Converts the key code to a char
    //
    auto to_char() const noexcept
    {
      return static_cast<std::uint8_t>(code);
    }

    //
    // Returns the button string identifier
    //
    auto to_string() const noexcept
    {
      return input_map::to_string(code);
    }

    //
    // Current button state
    //
    state keyState{};

    //
    // Engine key code
    //
    key_code code{};
  };


  //
  // Represents a 2D position of things like the mouse cursor,
  // gamepad sticks, or josticks
  // The values must be normalised in range [-1.0, 1.0]
  //
  struct position : public detail::device_idx
  {
    //
    // Event source type
    //
    using input_src = input_map::position_src;
    using enum input_src;

    CLASS_SPECIALS_NODEFAULT(position);

    position(idx_type devIdx, coord_type x, coord_type y, input_src src) noexcept :
      detail::device_idx{ devIdx },
      horizontal{ x },
      vertical{ y },
      source{ src }
    {}

    //
    // Returns position source as string
    //
    auto to_string() const noexcept
    {
      return input_map::to_string(source);
    }

    //
    // Horizontal position component
    //
    coord_type horizontal{};

    //
    // Vertical position component
    //
    coord_type vertical{};

    //
    // Input source
    //
    input_src source{};
  };


  //
  // Axis event (mouse wheel, gamepad triggers, etc.)
  // Must be in range [-1.0, 1.0]
  //
  struct axis : public detail::device_idx
  {
    //
    // Event source type
    //
    using input_src = input_map::axis_src;
    using enum input_src;

    axis(idx_type devIdx, coord_type d, input_src src) noexcept :
      detail::device_idx{ devIdx },
      delta{ d },
      source{ src }
    {}

    //
    // Returns axis source as string
    //
    auto to_string() const noexcept
    {
      return input_map::to_string(source);
    }

    //
    // Axis delta
    //
    coord_type delta{};

    //
    // Input source
    //
    input_src source{};
  };
}