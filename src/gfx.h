#ifndef GFX
#define GFX

#include <SDL2/SDL_image.h>
#include "ui.h"

//size to draw each cell in pixels
#define CELLSIZE 2

#define SCREENWIDTH 1600
#define SCREENHEIGHT 900

void GFXSetDefaults();

struct colors{
	SDL_Color red;
	SDL_Color blue;
	SDL_Color green;
	SDL_Color black;
	SDL_Color white;
	SDL_Color dark_grey;
	SDL_Color light_grey;
};

struct button_color_defaults{
	button_color unselected;
	button_color nothing_selected;
	button_color nothing_hovered;
	button_color sand_selected;
	button_color sand_hovered;
	button_color red_sand_selected;
	button_color red_sand_hovered;
	button_color water_selected;
	button_color water_hovered;
	button_color oil_selected;
	button_color oil_hovered;
	button_color grid_clear_hovered;
	button_color grid_clear_pushed;
	button_color game_quit_hovered;
	button_color game_quit_pushed;
};

extern struct colors color_palette;
extern struct button_color_defaults button_color_palette;

#endif
