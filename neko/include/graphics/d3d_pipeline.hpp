#pragma once
#include "graphics/pipeline.hpp"

#if NEK_WINDOWS
  #include "windows/window.hpp"
  #include "platform/support/directx/d3d12.h"
  #include "platform/support/directx/d3dx12.h"
  #include <dxgi1_6.h>
  #include <wrl.h>
  #include <d3dcompiler.h>
  #include <directxmath.h>

  #pragma comment (lib, "d3d12.lib")
  #pragma comment (lib, "dxgi.lib")
  #pragma comment (lib, "d3dcompiler.lib")
#else
  NEK_BAD_PLATFORM
#endif

namespace neko::platform
{
  class d3d : public pipeline
  {
  public:
    CLASS_SPECIALS_NONE(d3d);

    virtual ~d3d() noexcept;

    d3d(const host_info& info) noexcept;

  public:
    virtual bool good() noexcept override;

  private:
    host_info m_host{};
  };
}