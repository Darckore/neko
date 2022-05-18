#include "graphics/d3d_pipeline.hpp"

namespace neko::platform
{
  // Additional definitions

  namespace detail
  {
    //---------------------------------------
    // DirectX render target
    //---------------------------------------

    // Special members

    d3d_target::~d3d_target() noexcept
    {
      logger::note("Shutting down Direct3D pipeline");
      shutdown();
    }

    d3d_target::d3d_target(const host_info& info) noexcept :
      m_host{ info }
    {
      logger::note("Initialising Direct3D pipeline");
    }

    d3d_target::operator bool() const noexcept
    {
      return static_cast<bool>(m_device);
    }

    // Public members

    bool d3d_target::init() noexcept
    {
      return true;
    }

    // Private members
    void d3d_target::shutdown() noexcept
    {

    }

  }

  // Special members

  d3d::~d3d() noexcept = default;

  d3d::d3d(const host_info& info) noexcept :
    m_target{ info }
  {
    if (!m_target.init())
    {
      logger::error("Direct3D initialisation failed");
      return;
    }
  }

  // Public members

  bool d3d::good() noexcept
  {
    return static_cast<bool>(m_target);
  }
}