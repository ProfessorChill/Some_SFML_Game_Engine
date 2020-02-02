#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <boost/filesystem/path.hpp>
#include <iostream>
#include <map>
#include <stack>
#include <string>

// #include "common_functions.hpp"

class GameState;

class Game
{
    public:
	Game();

	sf::RenderWindow window;

	std::stack<GameState *> states;
	void pushState(GameState *state);
	void popState();
	void changeState(GameState *state);
	GameState *peekState();

	boost::filesystem::path selfLocation;

	void gameLoop();

    private:
	void loadTextures();
};

#endif
