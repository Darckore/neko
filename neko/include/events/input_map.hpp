#pragma once

namespace neko::evt
{
  class input_map final
  {
  public:
    using raw_code    = std::uint16_t;

    enum class key_codes : std::uint8_t
    {
      DEAD,

      // Mouse buttons
      MOUSE_L,
      MOUSE_R,
      MOUSE_M,

      // Numeric keys
      KB_0,
      KB_1,
      KB_2,
      KB_3,
      KB_4,
      KB_5,
      KB_6,
      KB_7,
      KB_8,
      KB_9,

      // Numpad keys
      KB_NUM_0,
      KB_NUM_1,
      KB_NUM_2,
      KB_NUM_3,
      KB_NUM_4,
      KB_NUM_5,
      KB_NUM_6,
      KB_NUM_7,
      KB_NUM_8,
      KB_NUM_9,

      // Alpha
      KB_A,
      KB_B,
      KB_C,
      KB_D,
      KB_E,
      KB_F,
      KB_G,
      KB_H,
      KB_I,
      KB_J,
      KB_K,
      KB_L,
      KB_M,
      KB_N,
      KB_O,
      KB_P,
      KB_Q,
      KB_R,
      KB_S,
      KB_T,
      KB_U,
      KB_V,
      KB_W,
      KB_X,
      KB_Y,
      KB_Z,

      // Functional keys
      KB_F1,
      KB_F2,
      KB_F3,
      KB_F4,
      KB_F5,
      KB_F6,
      KB_F7,
      KB_F8,
      KB_F9,
      KB_F10,
      KB_F11,
      KB_F12,
      KB_F13,
      KB_F14,
      KB_F15,
      KB_F16,
      KB_F17,
      KB_F18,
      KB_F19,
      KB_F20,
      KB_F21,
      KB_F22,
      KB_F23,
      KB_F24,

      // Special keys
      KB_ENTER,
      KB_ESC,
      KB_SPACE,
      KB_BACK,
      KB_TAB,
      KB_LEFT,
      KB_RIGHT,
      KB_UP,
      KB_DOWN
    };

    using enum key_codes;

  public:
    CLASS_SPECIALS_NONE(input_map);

  public:
    key_codes convert(raw_code code) noexcept;
  };
}