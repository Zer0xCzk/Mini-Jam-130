#pragma once

#include "SDL_main.h"
#include "SDL.h"
#include <stdbool.h>

enum dir { Left, Right };
enum type { Bat, Goblin };

struct Enemy {
	SDL_Rect body;
	SDL_Texture* texture;
	type type;
	dir direction;
	bool alive;
	double speed;
};