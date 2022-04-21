#include "windows/window.hpp"

#if NEK_WINDOWS

#include "managers/sys_registry.hpp"
#include "managers/logger.hpp"

namespace neko::platform
{
  // Additional definitions

  namespace detail
  {
    struct msg_wrapper
    {
      HWND handle;
      UINT msg_code;
      WPARAM wp;
      LPARAM lp;
    };

    struct wnd_helper
    {
      static constexpr auto windowClass = "MAINWND"sv;

      static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
      {
        window* wndPtr = nullptr;
        if (msg == WM_NCCREATE)
        {
          wndPtr = reinterpret_cast<window*>(((LPCREATESTRUCT)lParam)->lpCreateParams);
          SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(wndPtr));
          return TRUE;
        }

        if (msg != WM_TIMER)
        {
          wndPtr = reinterpret_cast<window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
          if (wndPtr)
          {
            return wndPtr->window_proc({ hwnd, msg, wParam, lParam });
          }
        }

        return DefWindowProc(hwnd, msg, wParam, lParam);
      }
      static auto make_wnd_class(std::string_view className) noexcept
      {
        auto inst_handle = GetModuleHandle(0);

        WNDCLASSEX wc = {};
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = wnd_proc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = inst_handle;
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wc.hIconSm = wc.hIcon;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
        wc.lpszMenuName = NULL;
        wc.lpszClassName = className.data();

        if (!RegisterClassEx(&wc))
        {
          NEK_TRACE("RegisterClassEx failed with code {}", GetLastError());
          return decltype(inst_handle){};
        }

        return inst_handle;
      }
      static auto calc_size() noexcept
      {
        MONITORINFO monitorInfo{ sizeof(monitorInfo) };
        auto monitorHandle = MonitorFromPoint(POINT{}, MONITOR_DEFAULTTOPRIMARY);
        GetMonitorInfo(monitorHandle, &monitorInfo);

        const auto& monitorRect = monitorInfo.rcMonitor;
        const auto posX = monitorRect.left;
        const auto posY = monitorRect.top;
        const auto width = monitorRect.right - posX;
        const auto height = monitorRect.bottom - posY;
        return RECT{ posX, posY, width, height };
      }
    };
  }

  // Window procedure

  window::proc_result window::window_proc(msg_wrapper msg) noexcept
  {
    auto [handle, msg_code, wp, lp] = msg;
    switch (msg_code)
    {
    case WM_SIZE:
      m_size = { GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };
      return 0;

    case WM_DESTROY:
      close();
      return 0;

    case WM_KEYDOWN:
      if (!(lp & 0x40000000)) // No autorepeat
      {
        //
      }
      return 0;

    case WM_KEYUP:
      return 0;
    }

    return DefWindowProc(handle, msg_code, wp, lp);
  }

  // Special members

  window::~window() noexcept
  {
    close();
  }

  window::window() noexcept
  {
    init();
  }

  window::operator bool() const noexcept
  {
    return static_cast<bool>(m_handle);
  }

  // Public members

  void window::close() noexcept
  {
    if (!m_handle)
    {
      return;
    }

    NEK_TRACE("Closing window");
    using detail::wnd_helper;

    SetWindowLongPtr(m_handle, GWLP_USERDATA, 0);

    auto inst_handle = GetModuleHandle(0);
    DestroyWindow(m_handle);

    m_handle = 0;
    UnregisterClass(wnd_helper::windowClass.data(), inst_handle);
  }

  bool window::update() noexcept
  {
    if (!*this)
    {
      NEK_TRACE("Window is about to close");
      return false;
    }

    MSG msg{};
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    return msg.message != WM_QUIT;
  }

  window::dimensions window::size() const noexcept
  {
    return m_size;
  }

  window::handle_type window::handle() const noexcept
  {
    return m_handle;
  }

  // Private members

  void window::init() noexcept
  {
    NEK_TRACE("Window init");

  #ifdef NDEBUG
    ShowWindow(GetConsoleWindow(), SW_HIDE);
  #endif

    using detail::wnd_helper;
    auto className   = wnd_helper::windowClass.data();
    auto inst_handle = wnd_helper::make_wnd_class(className);
    NEK_TRACE("Window class {}", wnd_helper::windowClass);
    if (!inst_handle)
    {
      logger::error("Unable to register window class");
      return;
    }

    NEK_TRACE("Window class registered");
    auto [posX, posY, width, height] = wnd_helper::calc_size();
    m_size = { width, height };
    NEK_TRACE("Window size: [{}, {}]", width, height);
    auto handle = CreateWindow(
      className,
      nullptr,
      WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
      posX, posY, width, height,
      NULL, NULL,
      inst_handle,
      this
    );
    
    if (!handle)
    {
      logger::error("Window creation failed");
      return;
    }

    NEK_TRACE("Window created");
    SetWindowPos(handle, HWND_TOP, posX, posY, width, height,
                 SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

    ShowWindow(handle, SW_SHOW);
    UpdateWindow(handle);
    m_handle = reinterpret_cast<handle_type>(handle);
    NEK_TRACE("Done init window");
  }
}

#endif