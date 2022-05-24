#ifndef CELLS
#define CELLS

#include "anna-layer.h"
#include <SDL2/SDL_render.h>

#define GRIDWIDTH 256
#define GRIDHEIGHT 256

typedef enum {
  // order in this list is important, it implies the density.
  // Lower -> Higher Density (it will have a higher enum value)
  // density matters because it determines where they will sit eventually
  // overall GAS < LIQUID < SOLID. solids do not pass through eachother
  // from the density, but gasses and liquids do.

    // gasses
	CELL_NOTHING,
    // liquids
	CELL_OIL,
	CELL_WATER,
	// solids
	CELL_RED_SAND,
	CELL_SAND,
	//blocker (leave at bottom or bad things will happen)
	CELL_BLOCKER
}cell_id;

typedef struct cell_material_t{
	cell_id id; // higher number = higher density
	char solid;
	char liquid;
	uint liquid_spread_factor; // only relevant for liquids
	char gas;
	char flammable;
	char finite_frames_to_live;
	SDL_Color color;
}cell_material;    

typedef struct cell_t{
	cell_material* material;
	uint frames_to_live;
	char moved_this_frame;
}cell;

typedef struct{
	cell_material blocker;
	cell_material nothing;
	cell_material red_sand;
	cell_material sand;
	cell_material water;
	cell_material oil;
}cell_materials;


extern cell_materials cell_mats;
extern cell cell_buffer[GRIDWIDTH][GRIDHEIGHT];


void grid_draw(SDL_Renderer* r);
void grid_update();

#endif
