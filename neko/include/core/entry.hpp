//
// Entry point for the game.
// Applications must include this instead of defining
// their own main entry point
// 
// The make_game function should be defined by using the
// NEK_REGISTER_GAME macro
// Use it outside of any namespace in a cpp file and specify
// a fully qualified name of the derived class as its parameter
//
// -------------------------------------------------------------
// Example:
//
//    mygame.h:
//
//    #include "core/entry.hpp"
//
//    namespace something
//    {
//       class mygame : public neko::base_game { ... };
//    }
//
//    mygame.cpp
//    NEK_REGISTER_GAME(something::mygame, "path/to/roor/config.cfg")
//
//    namespace something { definitions }
//
// -------------------------------------------------------------
//

#pragma once
#include "core/definitions.hpp"
#include "game/base_game.hpp"

int main()
{
  utils::set_terminate(-1, neko::on_terminate);
  neko::logger::init();
  NEK_TRACE("Logger ready");

  auto game = neko::make_game();
  if (!game)
  {
    neko::logger::error("Game creation failed");
    return -1;
  }

  neko::core::startup();

  return 0;
}