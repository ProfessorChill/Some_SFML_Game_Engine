#ifndef COMPONENTS_MOVEMENT_HPP
#define COMPONENTS_MOVEMENT_HPP

#include "../tmx-parser/map.hpp"

struct MovementNew {
	bool up;
	bool right;
	bool down;
	bool left;
	bool running;
	tmx::Map *mapPtr; // For collision checking and event checking (if needed).
};

#endif
