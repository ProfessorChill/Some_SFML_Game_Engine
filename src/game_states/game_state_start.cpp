#include "game_state_start.hpp"

GameStateStart::GameStateStart(Game *game)
{
	// Initialize the game reference
	this->game = game;

	// Initialize the map
	this->map = tmx::Map(this->game->selfLocation.string(),
			     "resources/maps/untitled.tmx");

	std::cout << "Map initialized\n";

	// Setup the initial game view
	sf::Vector2f pos = sf::Vector2f(this->game->window.getSize());
	this->gameView.setSize(pos);
	pos *= 0.5f;
	this->gameView.setCenter(pos);

	std::cout << "Gameview setup\n";

	// Load the textures
	this->loadTextures();

	std::cout << "Textures loaded\n";

	// Handle loading of entities (usually for testing)
	this->entityHandler.addEntity(this->texmgr.getRef("male_test"));
	// this->entityHandler.addEntity(this->texmgr.getRef("magecity_1"));

	std::cout << "Loaded entities\n";

	// Handle the lua state
	this->lua.open_libraries(sol::lib::base, sol::lib::package);

	sol::usertype<Game> gameType =
	    this->lua.new_usertype<Game>("Game", sol::constructors<Game()>());

	gameType["states"] = &Game::states;
	gameType["pushState"] = &Game::pushState;
	gameType["popState"] = &Game::popState;
	gameType["changeState"] = &Game::changeState;
	gameType["peekState"] = &Game::peekState;

	sol::usertype<EntityHandler> entityHandlerType =
	    this->lua.new_usertype<EntityHandler>(
		"EntityHandler", sol::constructors<EntityHandler()>());

	entityHandlerType["addEntity"] = &EntityHandler::addEntity;
	entityHandlerType["getEntityById"] = &EntityHandler::getEntityById;

	sol::usertype<GameStateStart> gameStateStartType =
	    this->lua.new_usertype<GameStateStart>(
		"GameStateStart", sol::constructors<GameStateStart(Game *)>());

	gameStateStartType["entityHandler"] = &GameStateStart::entityHandler;

	sol::usertype<sf::Vector2f> vector2fType =
	    this->lua.new_usertype<sf::Vector2f>(
		"Vector2f", sol::constructors<sf::Vector2f(),
					      sf::Vector2f(float, float)>());

	vector2fType["x"] = &sf::Vector2f::x;
	vector2fType["y"] = &sf::Vector2f::y;

	sol::usertype<Entity> entityType = this->lua.new_usertype<Entity>(
	    "Entity", sol::constructors<Entity(), Entity(sf::Texture * texture,
							 unsigned int)>());

	entityType["pos"] = &Entity::pos;
	entityType["id"] = &Entity::id;

	sol::usertype<TextureManager> textureManagerType =
	    this->lua.new_usertype<TextureManager>(
		"TextureManager", sol::constructors<TextureManager()>());

	textureManagerType["loadTexture"] = &TextureManager::loadTexture;
	textureManagerType["getRef"] = &TextureManager::getRef;

	this->lua["game"] = game;
	this->lua["entityHandler"] = &this->entityHandler;
	this->lua["textureManager"] = &this->texmgr;
	this->lua["selfPath"] = this->game->selfLocation.string().c_str();

	this->lua.script_file(this->game->selfLocation.string() +
			      "/resources/scripts/test.lua");

	std::cout << "Initialized LUA\n";
}

void GameStateStart::draw(const sf::Time deltaTime)
{
	this->game->window.clear(sf::Color::Black);

	this->game->window.setView(this->gameView);

	this->map.draw(this->game->window, deltaTime);

	this->entityHandler.draw(this->game->window, deltaTime);
}

void GameStateStart::update(const sf::Time deltaTime)
{
	// float dt = deltaTime.asSeconds();

	this->entityHandler.update(deltaTime);

	// Handle collision
	Entity *controlledEntity = this->entityHandler.getEntityById(0);
	size_t entitiesLen = this->entityHandler.getEntityCount();

	for (size_t i = 0; i < entitiesLen; i++) {
		if (i == 0) { // If the entity is the controlled entity
			continue;
		}

		Entity *queryEntity = this->entityHandler.getEntityById(i);

		if (queryEntity->collRect.intersects(
			controlledEntity->collRect)) {
			controlledEntity->pos = controlledEntity->lastPos;

			/* Testing bottom position movement */
			controlledEntity->pos.y =
			    queryEntity->collRect.top +
			    controlledEntity->collRect.height;
			controlledEntity->vel.y = 0.f;
			/* End testing */

			std::cout
			    << "Player Top: " << controlledEntity->collRect.top
			    << " Entity Bottom: "
			    << queryEntity->collRect.top +
				   queryEntity->collRect.height
			    << " Rect Size: "
			    << sizeof(controlledEntity->collRect) << "\n";
			break;
		}
	}

	// Set the view to the controlled entity
	this->gameView.setCenter(controlledEntity->pos);
}

void GameStateStart::handleInput()
{
	sf::Event event;

	while (this->game->window.pollEvent(event)) {
		switch (event.type) {
		case sf::Event::Closed: {
			this->game->window.close();

			break;
		}

		case sf::Event::Resized: {
			this->gameView.setSize(event.size.width,
					       event.size.height);

			break;
		}

		case sf::Event::KeyPressed: {
			if (event.key.code == sf::Keyboard::Escape) {
				this->game->window.close();
			}

			Entity *controlledEntity =
			    this->entityHandler.getEntityById(0);
			controlledEntity->keyPress(event.key.code);

			break;
		}

		case sf::Event::KeyReleased: {
			Entity *controlledEntity =
			    this->entityHandler.getEntityById(0);
			controlledEntity->keyRelease(event.key.code);

			break;
		}

		default:
			break;
		}
	}
}

void GameStateStart::loadTextures()
{
	// Load all of the textures in the game state

	texmgr.loadTexture("magecity_1",
			   this->game->selfLocation.string() +
			       "/resources/spritesheets/magecity_1.png");

	texmgr.loadTexture("oga_preview",
			   this->game->selfLocation.string() +
			       "/resources/spritesheets/oga_preview.png");

	texmgr.loadTexture("male_test",
			   this->game->selfLocation.string() +
			       "/resources/spritesheets/male_sprites.png");
}
