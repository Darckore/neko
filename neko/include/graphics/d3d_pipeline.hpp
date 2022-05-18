//
// DirectX 12 support
//

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
  namespace detail
  {
    //
    // Type for size and indexing
    //
    using size_type = std::uint64_t;

    //
    // An alias for MS com pointer
    //
    template <typename T>
    using com_ptr = Microsoft::WRL::ComPtr<T>;

    //
    // We are using tripple buffering here
    //
    inline constexpr auto bufCnt = 3u;

    //
    // Storage type primarily for per-buffer stuff
    //
    template <typename T>
    using arr_type = std::array<T, bufCnt>;

    //
    // Just a convenient alias for an array of com ptrs
    //
    template <typename T>
    using ptr_arr = arr_type<com_ptr<T>>;

    //
    // DirectX render target
    //
    class d3d_target final
    {
    public:
      CLASS_SPECIALS_NONE(d3d_target);

      ~d3d_target() noexcept;

      explicit d3d_target(const host_info& info) noexcept;

      //
      // Checks whether the target is valid
      //
      explicit operator bool() const noexcept;

    public:
      //
      // Initialises the target
      // Doing it here rather than in the constructor
      // for better control
      //
      bool init() noexcept;

    private:
      //
      // Uninitialises the pipeline
      //
      void shutdown() noexcept;

    private:
      //
      // App host information
      //
      const host_info& m_host;

      com_ptr<IDXGIFactory4>               m_factory{};
      com_ptr<IDXGIAdapter4>               m_adapter{};
      com_ptr<ID3D12Device2>               m_device{};
      com_ptr<ID3D12CommandQueue>          m_cmdQueue{};
      com_ptr<IDXGISwapChain4>             m_swapChain{};
      com_ptr<ID3D12GraphicsCommandList>   m_cmdList{};
      com_ptr<ID3D12DescriptorHeap>        m_descriptorHeap{};

      ptr_arr<ID3D12Resource>         m_backBuffers{};
      ptr_arr<ID3D12CommandAllocator> m_cmdAllocators{};

      size_type m_RTVDescriptorSize{};
      size_type m_currentBuf{};

      ptr_arr<ID3D12Fence> m_fences{};
      arr_type<size_type> m_fenceVals{};
      HANDLE m_fenceEvent{};
    };
  }

  //
  // DirectX 12 pipeline implementation
  //
  class d3d final : public pipeline
  {
  public:
    CLASS_SPECIALS_NONE(d3d);

    virtual ~d3d() noexcept;

    explicit d3d(const host_info& info) noexcept;

  public:
    //
    // Checks that the pipeline is valid and ready
    //
    virtual bool good() noexcept override;

  private:
    //
    // Render target
    //
    detail::d3d_target m_target;
  };
}