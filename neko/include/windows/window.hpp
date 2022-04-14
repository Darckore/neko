#pragma once
#include "platform/support/windows/win_includes.hpp"
#include "core/draw_target.hpp"

namespace neko::platform
{
  namespace detail
  {
    struct msg_wrapper;
    struct wnd_helper;
  }

  class window final : public draw_target
  {
  public:
    using proc_result   = std::intptr_t;
    using handle_type   = HWND;

  private:
    using msg_wrapper = detail::msg_wrapper;
    friend struct detail::wnd_helper;
    friend class singleton<draw_target>;

  public:
    CLASS_SPECIALS_NONE_CUSTOM(window);

    ~window() noexcept;

    explicit operator bool() const noexcept;

  private:
    window() noexcept;

  private:
    proc_result window_proc(msg_wrapper msg) noexcept;

  public:
    virtual bool update() noexcept override;
    void close() noexcept;

    virtual dimensions size() const noexcept override;

    handle_type handle() const noexcept;

  private:
    void init() noexcept;

  private:
    handle_type m_handle{};
    dimensions  m_size{};
  };
}