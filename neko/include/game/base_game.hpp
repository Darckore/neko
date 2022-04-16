#pragma once
#include "core/core.hpp"

namespace neko
{
  class base_game
  {
  public:
    friend class core;

    using time_type = neko::time_type;
    
  public:
    CLASS_SPECIALS_NONE_CUSTOM(base_game);

    virtual ~base_game() noexcept;

    base_game() noexcept;

  protected:
    virtual bool load() noexcept = 0;
    virtual void on_update(time_type dt) noexcept = 0;
    virtual void on_render() noexcept = 0;

  private:
    bool init() noexcept;
    void update(time_type dt) noexcept;
    void render() noexcept;

  protected:
    void run() noexcept;
    void quit() noexcept;
  };

}

namespace neko_game
{
  using game_ptr = neko::pointer<neko::base_game>;
  game_ptr make_game();
}