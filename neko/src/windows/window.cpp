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

      template <utils::detail::integer ...Args>
      constexpr bool is(Args ...args) const noexcept
      {
        using ct = std::common_type_t<decltype(msg_code), Args...>;
        return utils::eq_any(static_cast<ct>(msg_code), static_cast<ct>(args)...);
      }
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
    case WM_KEYUP:
      on_key(msg);
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

    kb::dispatch();
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

  /*
  * Processed keys:
  * 
  * 0x30  0
  * 0x31  1
  * 0x32  2
  * 0x33  3
  * 0x34  4
  * 0x35  5
  * 0x36  6
  * 0x37  7
  * 0x38  8
  * 0x39  9
  * 
  * 0x41  A
  * 0x42  B
  * 0x43  C
  * 0x44  D
  * 0x45  E
  * 0x46  F
  * 0x47  G
  * 0x48  H
  * 0x49  I
  * 0x4A  J
  * 0x4B  K
  * 0x4C  L
  * 0x4D  M
  * 0x4E  N
  * 0x4F  O
  * 0x50  P
  * 0x51  Q
  * 0x52  R
  * 0x53  S
  * 0x54  T
  * 0x55  U
  * 0x56  V
  * 0x57  W
  * 0x58  X
  * 0x59  Y
  * 0x5A  Z
  * 
  * 0x60 VK_NUMPAD0
  * 0x61 VK_NUMPAD1
  * 0x62 VK_NUMPAD2
  * 0x63 VK_NUMPAD3
  * 0x64 VK_NUMPAD4
  * 0x65 VK_NUMPAD5
  * 0x66 VK_NUMPAD6
  * 0x67 VK_NUMPAD7
  * 0x68 VK_NUMPAD8
  * 0x69 VK_NUMPAD9
  * 
  * 0x08 VK_BACK (BACKSPACE)
  * 0x09 VK_TAB
  * 
  * 0x0D VK_RETURN
  * 
  * 0x10 VK_SHIFT
  * 0x11 VK_CONTROL
  * 0x12 VK_MENU (ALT)
  * 0x13 VK_PAUSE
  * 
  * 0x1B VK_ESCAPE
  * 
  * 0x20 VK_SPACE
  * 0x21 VK_PRIOR (PAGE UP)
  * 0x22 VK_NEXT  (PAGE DOWN)
  * 0x23 VK_END
  * 0x24 VK_HOME
  * 0x25 VK_LEFT
  * 0x26 VK_UP
  * 0x27 VK_RIGHT
  * 0x28 VK_DOWN
  * 
  * 0x2C VK_SNAPSHOT (PRINT SCREEN)
  * 0x2D VK_INSERT
  * 0x2E VK_DELETE
  * 
  * 0x6A VK_MULTIPLY
  * 0x6B VK_ADD
  * 0x6D VK_SUBTRACT
  * 0x6E VK_DECIMAL
  * 0x6F VK_DIVIDE
  * 
  * 0x70 VK_F1
  * 0x71 VK_F2
  * 0x72 VK_F3
  * 0x73 VK_F4
  * 0x74 VK_F5
  * 0x75 VK_F6
  * 0x76 VK_F7
  * 0x77 VK_F8
  * 0x78 VK_F9
  * 0x79 VK_F10
  * 0x7A VK_F11
  * 0x7B VK_F12
  * 0x7C VK_F13
  * 0x7D VK_F14
  * 0x7E VK_F15
  * 0x7F VK_F16
  * 0x80 VK_F17
  * 0x81 VK_F18
  * 0x82 VK_F19
  * 0x83 VK_F20
  * 0x84 VK_F21
  * 0x85 VK_F22
  * 0x86 VK_F23
  * 0x87 VK_F24
  * 
  * 0x90 VK_NUMLOCK
  * 0x91 VK_SCROLL
  * 0x14 VK_CAPITAL
  */
  window::kc window::key_code(WPARAM code) const noexcept
  {
    const auto inputKey = static_cast<kc>(code);
    return inputKey;
  }

  void window::on_key(msg_wrapper msg) noexcept
  {
    const bool keyUp   = msg.is(WM_KEYUP, WM_SYSKEYUP);
    const bool keyDown = !keyUp && msg.is(WM_KEYDOWN, WM_SYSKEYDOWN)
                                && !(msg.lp & 0x40000000); // no autorepeat

    if (!(keyUp || keyDown))
    {
      return;
    }

    const auto kind = keyUp ? kb_evt::up : kb_evt::down;
    const auto code = key_code(msg.wp);
    kb::push(kind, code);
  }
}

#endif