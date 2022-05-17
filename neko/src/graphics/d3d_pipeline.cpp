#include "graphics/d3d_pipeline.hpp"

namespace neko::platform
{
  // Special members

  d3d::~d3d() noexcept
  {}

  d3d::d3d(const host_info& info) noexcept :
    m_host{ info }
  {
  }

  // Public members

  bool d3d::good() noexcept
  {
    return true;
  }
}