#pragma once

#include "SDL_main.h"
#include "SDL.h"
#include <stdbool.h>

struct Weapon {
	SDL_Texture* icon;
	unsigned int cooldown;
};