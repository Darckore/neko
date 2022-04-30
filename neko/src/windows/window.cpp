#include "windows/window.hpp"

#if NEK_WINDOWS

#include "managers/sys_registry.hpp"
#include "managers/logger.hpp"

namespace neko::platform
{
  // Additional definitions

  namespace detail
  {
    //
    // Message wrapper
    //
    struct msg_wrapper
    {
      HWND handle;
      UINT msg_code;
      WPARAM wp;
      LPARAM lp;

      //
      // Checks if the message is one of the given kinds
      //
      template <utils::detail::integer ...Args>
      constexpr bool is(Args ...args) const noexcept
      {
        using ct = std::common_type_t<decltype(msg_code), Args...>;
        return utils::eq_any(static_cast<ct>(msg_code), static_cast<ct>(args)...);
      }

    private:
      //
      // Makes mouse button codes consistent
      //
      msg_wrapper& norm_mouse() noexcept
      {
        switch (msg_code)
        {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
          wp = VK_LBUTTON;
          break;
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
          wp = VK_RBUTTON;
          break;
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
          wp = VK_MBUTTON;
          break;

        default:
          break;
        }

        return *this;
      }
      
      //
      // Converts specific up/down events to the generic
      // WM_KEYUP/WM_KEYDOWN
      //
      msg_wrapper& norm_up_down() noexcept
      {
        if (is(WM_SYSKEYDOWN, WM_LBUTTONDOWN, WM_RBUTTONDOWN, WM_MBUTTONDOWN))
        {
          msg_code = WM_KEYDOWN;
        }
        else if (is(WM_SYSKEYUP, WM_LBUTTONUP, WM_RBUTTONUP, WM_MBUTTONUP))
        {
          msg_code = WM_KEYUP;
        }

        return *this;
      }
      
      //
      // Produces distinction between left and right
      // shift/ctrl/alt
      //
      msg_wrapper& norm_modifiers() noexcept
      {
        const auto scancode = static_cast<UINT>((lp & 0x00ff0000) >> 16);
        const auto extended = (lp & 0x01000000) != 0;

        switch (wp)
        {
        case VK_SHIFT:
          wp = MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX);
          break;
        case VK_CONTROL:
          wp = extended ? VK_RCONTROL : VK_LCONTROL;
          break;
        case VK_MENU:
          wp = extended ? VK_RMENU : VK_LMENU;
          break;

        default:
          break;
        }

        return *this;
      }

    public:
      //
      // Prepares message data for further processing
      //
      msg_wrapper& normalise() noexcept
      {
        return norm_mouse()
              .norm_modifiers()
              .norm_up_down();
      }
    };


    //
    // Low-level init helper
    //
    struct wnd_helper
    {
      //
      // We always use the same window class because we'll have a single window
      //
      static constexpr auto windowClass = "NEKO_MAINWND"sv;

      //
      // Generic window procedure
      // Delegates calls to the specific window's proc
      //
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
      
      //
      // Registers the window class with the OS
      //
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
      
      //
      // Calculates window client rectangle
      //
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

    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
      on_key(msg);
      return DefWindowProc(handle, msg_code, wp, lp);

    case WM_KEYDOWN:
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_KEYUP:
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
      on_key(msg);
      return 0;

    case WM_MOUSEMOVE:
      on_mouse(msg);
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

    dispatch_events();
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

  void window::dispatch_events() noexcept
  {
    button::dispatch();
    position::dispatch();
  }
  void window::on_key(msg_wrapper msg) noexcept
  {
    msg.normalise();
    const bool keyUp   = msg.is(WM_KEYUP);
    const bool keyDown = !keyUp && msg.is(WM_KEYDOWN)
                                && !(msg.lp & 0x40000000); // no autorepeat

    if (!(keyUp || keyDown))
    {
      return;
    }

    using im = neko::evt::input_map;
    const auto state = keyUp ? btn_evt::up : btn_evt::down;
    const auto code = im::convert(msg.wp);
    button::push(state, code);
  }
  void window::on_mouse(msg_wrapper msg) noexcept
  {
    const auto halfScreenX = m_size.width / 2;
    const auto halfScreenY = m_size.height / 2;
    const auto mouseX = GET_X_LPARAM(msg.lp) - halfScreenX;
    const auto mouseY = halfScreenY - GET_Y_LPARAM(msg.lp);
    const auto normX = static_cast<coord_type>(mouseX) / halfScreenX;
    const auto normY = static_cast<coord_type>(mouseY) / halfScreenY;
    position::push(0u, normX, normY, pos_evt::MOUSE_PTR);
  }
}

#endif