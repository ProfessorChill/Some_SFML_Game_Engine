#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include <SFML/Graphics/View.hpp>

#include "game.hpp"

class GameState
{
public:
	virtual ~GameState()
	{
	}

	Game *game;

	virtual void draw(const sf::Time deltaTime) = 0;
	virtual void update(const sf::Time deltaTime) = 0;
	virtual void handleInput() = 0;
};

#endif
