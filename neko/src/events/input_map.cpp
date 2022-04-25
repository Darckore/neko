#include "events/input_map.hpp"

namespace neko::evt
{
  // Public members

  input_map::key_codes input_map::convert(raw_code code) noexcept
  {
    static constexpr std::array<key_codes, 256u> codes{
      DEAD,
    #if NEK_WINDOWS
      #include "windows/windows_keycodes.inl"
    #else
      static_assert(false, "Platform not supported");
    #endif
    };

    NEK_ASSERT(code < codes.size());
    return codes[code];
  }
}