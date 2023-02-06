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
//    NEK_REGISTER_GAME(something::mygame)
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
  std::set_terminate(neko::on_terminate);
  auto game = neko::make_game();
  return 0;
}