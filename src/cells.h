#ifndef CELLS
#define CELLS

#include "anna-layer.h"
#include <SDL2/SDL_render.h>

#define GRIDWIDTH 200
#define GRIDHEIGHT 200

typedef enum { CELL_BLOCKER, CELL_AIR, CELL_SAND }cell_t;

extern cell_t cell_buffer[GRIDWIDTH][GRIDHEIGHT];

void cellUpdateSand(int x, int y);
void gridDraw(SDL_Renderer* r);

#endif
