#pragma once
#include "core/singleton_base.hpp"
#include "managers/sys_registry.hpp"
#include "managers/logger.hpp"
#include "managers/config.hpp"

namespace neko
{
  class base_game;

  class core final : public singleton<core>
  {
  public:
    static void startup() noexcept;

  private:
    using game_type = base_game;
    friend class singleton<core>;

  public:
    CLASS_SPECIALS_NONE(core);

    ~core() noexcept;

  private:
    core(game_type& game, const path_type& cfgRoot) noexcept;

  private:
    void run() noexcept;
    void loop() noexcept;
    void quit() noexcept;

  private:
    game_type& m_game;
  };
}