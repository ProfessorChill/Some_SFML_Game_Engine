#include "game.hpp"
#include "game_state.hpp"

void Game::loadTextures() {}

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

		this->window.display();
	}
}

Game::Game()
{
	this->loadTextures();

	// this->selfLocation = get_selfpath();
	// std::cout << this->selfLocation << std::endl;

	this->window.create(sf::VideoMode(800, 600), "Some Game");
	this->window.setFramerateLimit(60);
}
