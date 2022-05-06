#include "core/managers.hpp"

#if NEK_WINDOWS
  // Windows pipelines go here
#else
  NEK_BAD_PLATFORM
#endif

namespace neko
{
  // Temporary garbage
  namespace platform
  {
    class pipeline {};
  }

  // Special members

  renderer::~renderer() noexcept
  {
    logger::note("Shutting down rendering pipeline");
  }

  renderer::renderer(const host_info& info) noexcept
  {
    utils::unused(info);
    logger::note("Initialising rendering pipeline");
  }

  renderer::operator bool() const noexcept
  {
    return true;
  }

  // Public members

  void renderer::render() noexcept
  {

  }
}