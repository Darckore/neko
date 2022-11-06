#include "graphics/d3d_pipeline.hpp"

//
// Checks whether the first parameter indicates success and posts an error
// message if it does not
// 
// The success is determined by either a HRESULT or bool check
// 
// This macro makes the enclosing function return false on failure
//
#define ENSURE(val, msg) { \
  if(!ensure_impl(val, msg)) {\
    return false; \
  }\
}

namespace neko::platform
{
  // Additional definitions

  namespace detail
  {
    inline bool ensure_impl(bool val, std::string_view errMsg) noexcept
    {
      if (!val)
      {
        logger::error(errMsg);
        return false;
      }
      return true;
    }

    inline bool ensure_impl(HRESULT hr, std::string_view errMsg) noexcept
    {
      if (FAILED(hr))
      {
        logger::error("{}. System error code: 0x{:04X}", errMsg, static_cast<DWORD>(hr));
        return false;
      }
      return true;
    }

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
      const bool success{
        enable_dbg() &&
        create_device()
      };

      if (!success)
      {
        m_device = nullptr;
      }

      return success;
    }

    // Private members

    void d3d_target::shutdown() noexcept
    {

    }

    bool d3d_target::enable_dbg() noexcept
    {
    #ifndef NDEBUG
      NEK_TRACE("Initialising D3D debug mode");
      com_ptr<ID3D12Debug> dbg;
      ENSURE(D3D12GetDebugInterface(IID_PPV_ARGS(&dbg)), "D3D debug mode failed");
      dbg->EnableDebugLayer();
    #endif
      return true;
    }

    bool d3d_target::create_device() noexcept
    {
      NEK_TRACE("Initialising D3D factory");

      constexpr std::uint32_t flags{
    #ifndef NDEBUG
        DXGI_CREATE_FACTORY_DEBUG
    #else
        0
    #endif
      };

      ENSURE(CreateDXGIFactory2(flags, IID_PPV_ARGS(&m_factory)), "D3D factory failed");

      auto vmem = 0ull;
      com_ptr<IDXGIAdapter1> adapter{};
      auto checkAdapter = [&vmem, &adapter](const DXGI_ADAPTER_DESC1& descr)
      {
        const auto isHW = (descr.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0;
        const auto devRes = D3D12CreateDevice(adapter.Get(),
                                              D3D_FEATURE_LEVEL_11_0,
                                              __uuidof(ID3D12Device),
                                              nullptr);
        const auto devOk = isHW && SUCCEEDED(devRes);
        return devOk && descr.DedicatedVideoMemory > vmem;
      };

      NEK_TRACE("Enumerating hardware adapters");
      bool hwAdapterFound{};
      for (auto idx = 0u; m_factory->EnumAdapters1(idx, &adapter) != DXGI_ERROR_NOT_FOUND; ++idx)
      {
        DXGI_ADAPTER_DESC1 adapterDescr;
        adapter->GetDesc1(&adapterDescr);

        if (checkAdapter(adapterDescr))
        {
          vmem = adapterDescr.DedicatedVideoMemory;
          hwAdapterFound = true;
        }
      }

      if (!hwAdapterFound)
      {
        logger::warning("Failed to find a suitable hardware D3D adapter. Falling back to WARP");
        ENSURE(m_factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter)), "Couldn't init WARP adapter");
      }

      NEK_TRACE("Initialising D3D device");
      ENSURE(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)), "D3D device failed");

    #ifndef NDEBUG
      if (com_ptr<ID3D12InfoQueue> infoQueue; SUCCEEDED(m_device.As(&infoQueue)))
      {
        NEK_TRACE("Setting D3D debug parameters");
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

        D3D12_MESSAGE_SEVERITY severities[]{ D3D12_MESSAGE_SEVERITY_INFO };
        D3D12_MESSAGE_ID ids[]{
            D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
            D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
            D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE
        };

        D3D12_INFO_QUEUE_FILTER filter{};
        filter.DenyList.NumSeverities = _countof(severities);
        filter.DenyList.pSeverityList = severities;
        filter.DenyList.NumIDs = _countof(ids);
        filter.DenyList.pIDList = ids;
        infoQueue->PushStorageFilter(&filter);
      }
    #endif

      m_RTVDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
      NEK_TRACE("RTV descriptor size: {}", m_RTVDescriptorSize);
      return true;
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