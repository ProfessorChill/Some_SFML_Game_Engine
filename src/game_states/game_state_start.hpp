#ifndef GAME_STATE_START_HPP
#define GAME_STATE_START_HPP

#include <SFML/Graphics.hpp>
#include <iostream>
#include <map>
#include <sol/sol.hpp>
#include <string>

#include "../entity-handler.hpp"
#include "../entity.hpp"
#include "../game.hpp"
#include "../game_state.hpp"
#include "../particle.hpp"
#include "../texture_manager.hpp"
#include "../tmx-parser/map.hpp"

class GameStateStart : public GameState
{
    public:
	explicit GameStateStart(Game *game);

	virtual void draw(const sf::Time deltaTime);
	virtual void update(const sf::Time deltaTime);
	virtual void handleInput();

	EntityHandler entityHandler;
	std::vector<ParticleSystem *> particleSystems;

	sol::state lua;

	TextureManager texmgr;

    private:
	void loadTextures();

	sf::View gameView;
	tmx::Map map;
};

#endif
