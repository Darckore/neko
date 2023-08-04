#include "events/input_map.hpp"

namespace neko::evt
{
  // Public members

  input_map::key_codes input_map::keyboard_convert(raw_code code) noexcept
  {
    static constexpr std::array codes{
      DEAD,
    #if NEK_WINDOWS
      #include "platform/windows/windows_keycodes.inl"
    #else
      NEK_BAD_PLATFORM
    #endif
    };

    NEK_ASSERT(code < codes.size());
    return codes[code];
  }

  input_map::key_name input_map::to_string(key_codes code) noexcept
  {
    static constexpr std::array names{
      "DEAD"sv,

      // Mouse buttons
      "MOUSE_L"sv,
      "MOUSE_R"sv,
      "MOUSE_M"sv,

      // Numeric keys
      "KB_0"sv,
      "KB_1"sv,
      "KB_2"sv,
      "KB_3"sv,
      "KB_4"sv,
      "KB_5"sv,
      "KB_6"sv,
      "KB_7"sv,
      "KB_8"sv,
      "KB_9"sv,

      // Numpad keys
      "KB_NUM_0"sv,
      "KB_NUM_1"sv,
      "KB_NUM_2"sv,
      "KB_NUM_3"sv,
      "KB_NUM_4"sv,
      "KB_NUM_5"sv,
      "KB_NUM_6"sv,
      "KB_NUM_7"sv,
      "KB_NUM_8"sv,
      "KB_NUM_9"sv,

      // Alpha
      "KB_A"sv,
      "KB_B"sv,
      "KB_C"sv,
      "KB_D"sv,
      "KB_E"sv,
      "KB_F"sv,
      "KB_G"sv,
      "KB_H"sv,
      "KB_I"sv,
      "KB_J"sv,
      "KB_K"sv,
      "KB_L"sv,
      "KB_M"sv,
      "KB_N"sv,
      "KB_O"sv,
      "KB_P"sv,
      "KB_Q"sv,
      "KB_R"sv,
      "KB_S"sv,
      "KB_T"sv,
      "KB_U"sv,
      "KB_V"sv,
      "KB_W"sv,
      "KB_X"sv,
      "KB_Y"sv,
      "KB_Z"sv,

      // Modifiers
      "KB_SHIFT"sv,
      "KB_CTRL"sv,
      "KB_ALT"sv,
      "KB_LSHIFT"sv,
      "KB_RSHIFT"sv,
      "KB_LCTRL"sv,
      "KB_RCTRL"sv,
      "KB_LALT"sv,
      "KB_RALT"sv,

      // Functional keys
      "KB_F1"sv,
      "KB_F2"sv,
      "KB_F3"sv,
      "KB_F4"sv,
      "KB_F5"sv,
      "KB_F6"sv,
      "KB_F7"sv,
      "KB_F8"sv,
      "KB_F9"sv,
      "KB_F10"sv,
      "KB_F11"sv,
      "KB_F12"sv,
      "KB_F13"sv,
      "KB_F14"sv,
      "KB_F15"sv,
      "KB_F16"sv,
      "KB_F17"sv,
      "KB_F18"sv,
      "KB_F19"sv,
      "KB_F20"sv,
      "KB_F21"sv,
      "KB_F22"sv,
      "KB_F23"sv,
      "KB_F24"sv,

      // Navigation keys
      "KB_ENTER"sv,
      "KB_ESC"sv,
      "KB_SPACE"sv,
      "KB_BACK"sv,
      "KB_TAB"sv,
      "KB_LEFT"sv,
      "KB_RIGHT"sv,
      "KB_UP"sv,
      "KB_DOWN"sv,
      "KB_PGUP"sv,
      "KB_PGDOWN"sv,
      "KB_END"sv,
      "KB_HOME"sv,
      "KB_INS"sv,
      "KB_DEL"sv,

      // Numpad operators
      "KB_MUL"sv,
      "KB_ADD"sv,
      "KB_SUB"sv,
      "KB_DIV"sv,
      "KB_SEP"sv,
      "KB_NUM_COMMA"sv,

      // Punctuation
      "KB_SEMICOLON"sv,
      "KB_PLUS"sv,
      "KB_MINUS"sv,
      "KB_COMMA"sv,
      "KB_DOT"sv,
      "KB_SLASH"sv,
      "KB_GRAVE"sv,
      "KB_OPEN_SQ"sv,
      "KB_CLOSE_SQ"sv,
      "KB_BACKSLASH"sv,
      "KB_QUOTE"sv,
      "KB_CLEAR"sv,

      // Caps, Num, Scroll lock
      "KB_CAPS"sv,
      "KB_NUM"sv,
      "KB_SCROLL"sv,

      // Misc
      "KB_PAUSE"sv,
      "KB_PRINT_SCREEN"sv,

      // Gamepads and joysticks

      // XBOX buttons
      "PAD_A"sv,
      "PAD_B"sv,
      "PAD_X"sv,
      "PAD_Y"sv,
      "PAD_LS"sv,
      "PAD_RS"sv,
      "PAD_LB"sv,
      "PAD_RB"sv,
      "PAD_START"sv,
      "PAD_BACK"sv,

      // D-pad
      "PAD_UP"sv,
      "PAD_DOWN"sv,
      "PAD_LEFT"sv,
      "PAD_RIGHT"sv,
    };

    static_assert(names.size() == static_cast<std::size_t>(CODE_LAST),
                  "There should be as many key names as there are key codes");

    const auto idx = static_cast<std::size_t>(code);
    NEK_ASSERT(idx < names.size());
    return names[idx];
  }

  input_map::src_name input_map::to_string(position_src src) noexcept
  {
    static constexpr std::array names{
      // Mouse pointer
      "Mouse"sv,

      // Gamepad sticks
      "Left stick"sv,
      "Right stick"sv,

      // Joystick
      "Joystick"sv
    };

    static_assert(names.size() == static_cast<std::size_t>(POS_SRC_LAST),
                  "There should be as many names as there are position sources");

    const auto idx = static_cast<std::size_t>(src);
    NEK_ASSERT(idx < names.size());
    return names[idx];
  }

  input_map::src_name input_map::to_string(axis_src src) noexcept
  {
    static constexpr std::array names{
      // Mouse wheel
      "Mouse wheel"sv,

      // Gamepad triggers
      "Left trigger"sv,
      "Right trigger"sv,
    };

    static_assert(names.size() == static_cast<std::size_t>(AXIS_SRC_LAST),
                  "There should be as many names as there are axis sources");

    const auto idx = static_cast<std::size_t>(src);
    NEK_ASSERT(idx < names.size());
    return names[idx];
  }
}