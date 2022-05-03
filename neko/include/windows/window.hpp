//
// Platform window
//

#pragma once

#if NEK_WINDOWS

#include "core/managers.hpp"
#include "platform/support/windows/win_includes.hpp"
#include "events/raw_input.hpp"

namespace neko::platform
{
  namespace detail
  {
    struct msg_wrapper;
    struct wnd_helper;
  }

  //
  // Window specific to the Windows platform
  //
  class window final : public app_host
  {
  public:
    //
    // Return type for the window procedure
    //
    using proc_result   = std::intptr_t;

    //
    // Windows handle
    //
    using handle_type   = HWND;

  private:
    //
    // A wrapper for a Windows message
    //
    using msg_wrapper = detail::msg_wrapper;

    //
    // A helper used for initialisation
    //
    friend struct detail::wnd_helper;

    friend class singleton<app_host>;

    //
    // Key up/down event
    //
    using btn_evt  = evt::button;
    using btn_code = btn_evt::key_code;
    using button   = event<btn_evt>;

    //
    // Mouse move event
    //
    using pos_evt  = evt::position;
    using position = event<pos_evt>;

    //
    // Mouse wheel event
    //
    using wheel_evt = evt::axis;
    using wheel     = event<wheel_evt>;

  public:
    CLASS_SPECIALS_NONE_CUSTOM(window);

    ~window() noexcept;

    //
    // Checks if the window is valid
    //
    explicit operator bool() const noexcept;

  private:
    window() noexcept;

  private:
    //
    // Window procedure
    //
    proc_result window_proc(msg_wrapper msg) noexcept;

  public:
    //
    // Processes events once per frame
    // Returns false if the window needs to close
    //
    virtual bool update() noexcept override;

    //
    // Closes the window and unregisters the window class
    //
    void close() noexcept;

    //
    // Returns window width and height
    //
    virtual dimensions size() const noexcept override;

    //
    // Returns window handle
    //
    handle_type handle() const noexcept;

  private:
    //
    // Initialises and displays the window
    //
    void init() noexcept;

    //
    // Dispatches events this frame
    //
    void dispatch_events() noexcept;

    //
    // Processes keyboard input and generates an event
    //
    void on_key(msg_wrapper msg) noexcept;

    //
    // Processes and normalises mouse cursor movement
    //
    void on_mouse(msg_wrapper msg) noexcept;

    //
    // Processes and normalises mouse wheel
    //
    void on_wheel(msg_wrapper msg) noexcept;

  private:
    //
    // Window handle
    //
    handle_type m_handle{};

    //
    // Window width and height
    //
    dimensions  m_size{};
  };
}

#endif