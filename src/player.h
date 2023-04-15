#pragma once

#include "SDL_main.h"
#include "SDL.h"
#include "item.h"
#include <stdbool.h>


struct Player {
	SDL_Texture* icon;
	SDL_Rect body;
	unsigned int hp;
};

