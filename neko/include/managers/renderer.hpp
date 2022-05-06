//
// Platform-independent renderer
//

#pragma once
#include "core/singleton_base.hpp"

namespace neko
{
  namespace platform
  {
    struct host_info;

    class pipeline;
  }

  //
  // Engine renderer
  // Provides general interface to implementation-defined graphics API
  //
  class renderer final : private singleton<renderer>
  {
  private:
    friend class singleton<renderer>;

  public:
    CLASS_SPECIALS_NONE(renderer);

    ~renderer() noexcept;

    //
    // Checks the renderer state
    // Returns false is the initialisation was unsuccessful
    //
    explicit operator bool() const noexcept;

  private:
    using host_info = platform::host_info;
    explicit renderer(const host_info& info) noexcept;

  public:
    //
    // Performs rendering commands
    //
    void render() noexcept;

  private:
    using pipeline = neko::pointer<platform::pipeline>;

    pipeline m_pipeline{};
  };
}