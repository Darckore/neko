#include "app/sandbox.hpp"

namespace neko_game
{
  // Test code
  struct evt_src
  {
    static inline int counter = 0;

    using et = neko::event<dummy_evt>;

    static void make() noexcept
    {
      et::push(counter, counter + 1);
      ++counter;
    }

    static void shoot() noexcept
    {
      if (counter > 10)
      {
        counter = 0;
        et::dispatch();
      }
    }
  };

  // Special members

  sandbox::~sandbox() noexcept = default;

  sandbox::sandbox() noexcept :
    m_sub{ this, [this](const auto& evt) noexcept { on_evt(evt); }}
  {
    NEK_TRACE("Sandbox go");
    run();
  }

  // Private members

  bool sandbox::load() noexcept
  {
    NEK_TRACE("Loading sandbox");
    NEK_TRACE("Setting sandox handler");
    return true;
  }
  void sandbox::on_update(time_type dt) noexcept
  {
    evt_src::make();
    evt_src::shoot();
    utils::unused(dt);
  }
  void sandbox::on_render() noexcept
  {

  }

  void sandbox::on_evt(const dummy_evt& e) noexcept
  {
    utils::unused(e);
  }

  game_ptr make_game()
  {
    return std::make_unique<sandbox>();
  }
}