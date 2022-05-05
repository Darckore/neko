#include "managers/renderer.hpp"
#include "core/managers.hpp"

namespace neko
{
  // Special members

  renderer::~renderer() noexcept
  {
    logger::note("Shutting down rendering pipeline");
  }

  renderer::renderer() noexcept
  {
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