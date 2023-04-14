#pragma once

#include "SDL_main.h"
#include "SDL.h"
#include <stdbool.h>

struct Item {
	SDL_Texture* icon;
	char headerstr[20];
	char descriptionstr[100];
	int price;
};