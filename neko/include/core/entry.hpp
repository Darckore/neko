//
// Entry point for the game.
// Applications must include this instead of defining
// their own main entry point
// 
// The make_game function should be defined in
// namespace neko_game
// It must return an std::unique_ptr to a class derived
// from neko::base_game
//

#pragma once
#include "game/base_game.hpp"

namespace neko_game
{
  extern game_ptr make_game();
}

int main()
{
  auto game = neko_game::make_game();
  return 0;
}