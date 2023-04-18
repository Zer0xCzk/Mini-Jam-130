#pragma once

#include "SDL_main.h"
#include "SDL.h"
#include <stdbool.h>

enum dir { Left, Right };
enum t { Bat, Goblin };

struct Enemy {
	SDL_Rect body;
	SDL_Texture* texture;
	t type;
	dir direction;
	bool alive;
	double speed;
};
