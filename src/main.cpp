#include "common_functions.hpp"
#include "game.hpp"
#include "game_states/game_ecs_test.hpp"
#include "game_states/game_state_start.hpp"

int main()
{
	Game game;
	game.selfLocation = get_selfpath();
	game.selfLocation = game.selfLocation.remove_filename();
	std::cout << game.selfLocation << std::endl;

	// game.pushState(new GameStateStart(&game));
	game.pushState(new GameEcsTest(&game));
	game.gameLoop();

	return 0;
}
