//
// Platform-independent renderer
//

#pragma once
#include "core/singleton_base.hpp"

namespace neko
{
  //
  // Engine renderer
  // Provides general interface to implementation-defined graphics API
  //
  class renderer final : private singleton<renderer>
  {
  private:
    friend class singleton<renderer>;

  public:
    CLASS_SPECIALS_NONE_CUSTOM(renderer);

    ~renderer() noexcept;

    //
    // Checks the renderer state
    // Returns false is the initialisation was unsuccessful
    //
    explicit operator bool() const noexcept;

  private:
    renderer() noexcept;

  public:
    //
    // Performs rendering commands
    //
    void render() noexcept;

  private:

  };
}