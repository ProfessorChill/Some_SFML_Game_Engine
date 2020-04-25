#include "game.hpp"
#include "game_state.hpp"

#include <sstream>

void Game::loadTextures()
{
}

void Game::pushState(GameState *state)
{
	// Add a state to the game

	this->states.push(state);
}

void Game::popState()
{
	// Remove the top state from the game

	delete this->states.top();
	this->states.pop();
}

void Game::changeState(GameState *state)
{
	// Change the game state from one to another

	if (!this->states.empty()) {
		popState();
	}

	pushState(state);
}

GameState *Game::peekState()
{
	// Get the current active state

	if (this->states.empty()) {
		return nullptr;
	}

	return this->states.top();
}

void Game::gameLoop()
{
	sf::Clock clock;

	// Temporary font for FPS display.
	sf::Font font;
	if (!font.loadFromFile(this->selfLocation.string() +
			       "/resources/fonts/Source Code Pro/SourceCodePro-Light.ttf")) {
		std::cout << "Unable to find font.\n";

		return;
	}

	sf::Text text;
	text.setFont(font);
	text.setString("FPS: 0.0000");
	text.setCharacterSize(16);
	text.setFillColor(sf::Color::White);

	while (this->window.isOpen()) {
		sf::Time elapsed = clock.restart();

		GameState *currentState = peekState();

		if (currentState == nullptr) {
			continue;
		}

		currentState->handleInput();
		currentState->update(elapsed);

		this->window.clear(sf::Color::Black);

		currentState->draw(elapsed);

		sf::Vector2f viewportTopLeft =
		    this->window.mapPixelToCoords(sf::Vector2i(0, 0), this->window.getView());

		std::ostringstream ss;
		ss << (1000000.0f / clock.restart().asMicroseconds());
		text.setString("FPS: " + ss.str());
		text.setPosition(viewportTopLeft);

		this->window.draw(text);

		this->window.display();
	}
}

Game::Game()
{
	this->loadTextures();

	// this->selfLocation = get_selfpath();
	// std::cout << this->selfLocation << std::endl;

	this->window.create(sf::VideoMode(800, 600), "Some Game");
	this->window.setVerticalSyncEnabled(true);
	this->window.setFramerateLimit(60);
}
