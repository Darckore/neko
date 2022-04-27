//
// Core module
//

#pragma once
#include "core/singleton_base.hpp"
#include "managers/sys_registry.hpp"
#include "managers/logger.hpp"
#include "managers/config.hpp"
#include "managers/event.hpp"

namespace neko
{
  class base_game;

  //
  // Core module
  // It handles most systems in the engine, and runs the main loop
  // Applications aren't supposed to interact with it directly
  //
  class core final : public singleton<core>
  {
  public:
    //
    // Main entry into the core
    // Called on game's initialisation
    //
    static void startup() noexcept;

  private:
    using game_type = base_game;
    friend class singleton<core>;

  public:
    CLASS_SPECIALS_NONE(core);

    ~core() noexcept;

  private:
    //
    // Accepts a reference to the game being run
    // The cfgRoot parameter is a path (without file name) to where
    // the root configuration file (root.cfg) is stored
    //
    core(game_type& game, const path_type& cfgRoot) noexcept;

  private:
    //
    // Ensures all systems are up and running, when goes into the main loop
    //
    void run() noexcept;

    //
    // This is the main loop of the game
    // Most of the activity is done here
    //
    void loop() noexcept;

    //
    // Shuts all systems down and exits the application
    //
    void quit() noexcept;

  private:
    //
    // User-defined game object
    //
    game_type& m_game;
  };
}