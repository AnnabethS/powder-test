#ifndef GFX
#define GFX

#include <SDL2/SDL_image.h>

//size to draw each cell in pixels
#define CELLSIZE 4

struct colors_t{
	SDL_Color sand;
	SDL_Color air;
	SDL_Color blocker;
};

extern struct colors_t default_colors;

void GFXSetDefaults();

#endif
