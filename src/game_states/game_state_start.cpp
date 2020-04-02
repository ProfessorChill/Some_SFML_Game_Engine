#include "game_state_start.hpp"

GameStateStart::GameStateStart(Game *game) : mEntities(ecs::MAX_ENTITIES - 1)
{
	// Initialize the game reference
	this->game = game;

	// Initialize the ECS
	mCoordinator.init();

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

	// Set the initial controlled entity.
	this->controlledEntity = this->entityHandler.getEntityById(0);
}

void GameStateStart::draw(const sf::Time deltaTime)
{
	// Store the temporary ID of the controlled ID
	// This is a janky fix until I change the code to use shared_ptr
	unsigned int tempId = this->controlledEntity->id;

	// Get viewport to rect.
	sf::Vector2f viewportTopLeft = this->game->window.mapPixelToCoords(
	    sf::Vector2i(0, 0), this->gameView);
	sf::Vector2f viewportSize = this->game->window.getView().getSize();
	sf::FloatRect viewport(viewportTopLeft.x, viewportTopLeft.y,
			       viewportSize.x, viewportSize.y);

	// Clear the window and set the view for drawing
	this->game->window.clear(sf::Color::Black);
	this->game->window.setView(this->gameView);

	this->map.draw(this->game->window, deltaTime);

	this->entityHandler.draw(this->game->window, deltaTime, viewport);
	/*
	std::vector<Entity *> visibleEntities =
	    this->entityHandler.getEntitiesInRegion(viewport);
	for (Entity *entity : visibleEntities) {
		entity->draw(this->game->window, deltaTime);
	}
	*/

	// Set the controlled entity to that temp ID, again, janky fix.
	this->controlledEntity = this->entityHandler.getEntityById(tempId);
}

void GameStateStart::update(const sf::Time deltaTime)
{
	// float dt = deltaTime.asSeconds();

	this->entityHandler.update(deltaTime);

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

				controlledEntity->keyPress(event.key.code);
				break;
			}

			case sf::Event::KeyReleased: {
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
