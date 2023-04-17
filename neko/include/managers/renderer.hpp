//
// Platform-independent renderer
//

#pragma once
#include "core/singleton_base.hpp"

namespace neko
{
  class renderer;
}

#include "graphics/pipeline.hpp"

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
    //
    // Initialises the rendering pipeline according to rendering settings
    //
    void init_pipeline(const host_info& info) noexcept;

  private:
    using pipeline = platform::pipeline;

    //
    // Platform-specific pipeline object
    //
    platform::pipeline_ptr m_pipeline{};
  };
}