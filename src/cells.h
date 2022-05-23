#ifndef CELLS
#define CELLS

#include "anna-layer.h"
#include <SDL2/SDL_render.h>

#define GRIDWIDTH 64
#define GRIDHEIGHT 64

typedef enum {
  // order in this list is important, it implies the density.
  // Lower -> Higher Density (it will have a higher enum value)
  // density matters because it determines where they will sit eventually
  // overall GAS < LIQUID < SOLID. solids do not pass through eachother
  // from the density, but gasses and liquids do.

    // gasses
	CELL_NOTHING,
    // liquids
	CELL_WATER,
	// solids
	CELL_SAND,
	//blocker (leave at bottom or bad things will happen)
	CELL_BLOCKER
}cell_id;

typedef struct cell_material_t{
	cell_id id;
	char solid;
	char liquid;
	char gas;
	char flammable;
	char finite_frames_to_live;
}cell_material;    

typedef struct cell_t{
	cell_material* material;
	uint frames_to_live;
}cell;

typedef struct{
	cell_material blocker;
	cell_material nothing;
	cell_material sand;
	cell_material water;
}cell_materials;


extern cell_materials cell_mats;
extern cell cell_buffer[GRIDWIDTH][GRIDHEIGHT];


void cellUpdateSand(int x, int y);
void gridDraw(SDL_Renderer* r);
void gridUpdate();

#endif
