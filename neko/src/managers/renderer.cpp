#include "core/managers.hpp"

#if NEK_WINDOWS
  #include "graphics/d3d_pipeline.hpp"
  using d3d = neko::platform::d3d;
#else
  NEK_BAD_PLATFORM
#endif

namespace neko
{
  // Special members

  renderer::~renderer() noexcept
  {
    logger::note("Shutting down rendering pipeline");
  }

  renderer::renderer(const host_info& info) noexcept
  {
    logger::note("Initialising rendering pipeline");
    init_pipeline(info);
  }

  renderer::operator bool() const noexcept
  {
    return static_cast<bool>(m_pipeline);
  }

  // Public members

  void renderer::render() noexcept
  {

  }

  // Private members

  void renderer::init_pipeline(const host_info& info) noexcept
  {
    m_pipeline = pipeline::create<d3d>(info);

    if (!m_pipeline || !m_pipeline->good())
    {
      logger::error("Pipeline initialisation failed");
      m_pipeline.reset();
      return;
    }
  }
}