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