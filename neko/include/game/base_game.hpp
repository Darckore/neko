//
// Game base class
//

#pragma once
#include "core/core.hpp"

namespace neko
{
  //
  // Base class for the game
  // Applications should derive from it
  // This class handles all the init/shutdown stuff outside core
  // Lifetime is managed in the application's entry point (see entry.hpp)
  //
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
    //
    // Application-specific initialisation
    // User games must override this and handle initial asset loading
    // Must return false to indicate load failure and an immediate shutdown
    //
    virtual bool load() noexcept = 0;

    //
    // Called every frame from the game loop
    // User games must provide implementation
    //
    virtual void on_update(time_type dt) noexcept = 0;

    //
    // Called every time the scene is being rendered
    //
    virtual void on_render() noexcept = 0;

  private:
    //
    // Makes sure thing are ready and then loads the game
    // Returns false on failure
    //
    bool init() noexcept;

    //
    // Called from the game loop
    // Performs common update logic and notifies the user
    //
    void update(time_type dt) noexcept;

    //
    // Called from the game loop
    // Performs common render logic and notifies the user
    //
    void render() noexcept;

  protected:
    //
    // Starts the engine
    // User game's constructor must call this as the last action
    //
    void run() noexcept;

    //
    // Shuts the engine down
    // User games must call this to exit
    //
    void quit() noexcept;
  };

}

namespace neko_game
{
  using game_ptr = neko::pointer<neko::base_game>;

  //
  // Creates the game
  // User games must provide implementation
  //
  game_ptr make_game();
}