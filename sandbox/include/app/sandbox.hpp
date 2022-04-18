#pragma once
#include "core/entry.hpp"

namespace neko_game
{
  struct dummy_evt
  {
    int one{};
    int two{};
  };

  class sandbox : public neko::base_game
  {
  public:
    CLASS_SPECIALS_NONE_CUSTOM(sandbox);

    ~sandbox() noexcept;

    sandbox() noexcept;

  private:
    virtual bool load() noexcept override;
    virtual void on_update(time_type dt) noexcept override;
    virtual void on_render() noexcept override;

  // Just fooling around
  private:
    void on_evt(const dummy_evt& e) noexcept;
    neko::event_subscriber<dummy_evt> m_sub;
  };

  game_ptr make_game();
}