#include "cells.h"
#include "anna-layer.h"
#include "gfx.h"
#include <SDL2/SDL_render.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string.h>

#define DEBUG_DRAW_GRID

// cell buffer that holds all of the cells, and is updated in place
cell cell_buffer[GRIDWIDTH][GRIDHEIGHT];
cell_materials cell_mats = {
    .blocker.id = CELL_BLOCKER,
    .blocker.solid = 1,
    .blocker.liquid = 0,
    .blocker.gas = 0,
    .blocker.flammable = 0,
    .blocker.finite_frames_to_live = 0,

    .nothing.id = CELL_NOTHING,
    .nothing.solid = 0,
    .nothing.liquid = 0,
    .nothing.gas = 0,
    .nothing.flammable = 0,
    .nothing.finite_frames_to_live = 0,

    .sand.id = CELL_SAND,
    .sand.solid = 1,
    .sand.liquid = 0,
    .sand.gas = 0,
    .sand.flammable = 0,
    .sand.finite_frames_to_live = 0,

    .water.id = CELL_WATER,
    .water.solid = 0,
    .water.liquid = 1,
    .water.gas = 0,
    .water.flammable = 0,
    .water.finite_frames_to_live = 0,
};


local cell_material* gridGetCellMaterial(int x, int y)
{
	if (x < 0 || y < 0 || x >= GRIDWIDTH || y >= GRIDHEIGHT)
	{
		return &cell_mats.nothing;
	}

	return cell_buffer[x][y].material;
}


void gridSolidUpdate(int x, int y)
{
	/*
	solid CA update options 

	Legend:
	. = AIR
	X = SOLID
	? = DONT CARE
	*/

		/*
		???    ???
		?X? -> ?X?
		XXX    XXX
		*/

		/*
		???    ???
		?X. -> ?..
		XX.    XXX
		*/

		/*
		???    ???
		.X? -> ..?
		.XX    XXX
		*/

		/*
		???    ???    ???
		.X. -> ... or ...
		.X.    XX.    .XX
		*/

		/*
		???    ???
		?X? -> ?.?
		?.?    ?X?
		*/
}

void gridUpdate()
{
	for(int x=0; x < GRIDWIDTH; x++)
	{
		for(int y=0; y < GRIDHEIGHT; y++)
		{
			if(cell_buffer[x][y].material->solid && !(cell_buffer[x][y].material->id==CELL_BLOCKER))
				gridSolidUpdate(x,y);
		}
	}
}

void gridDraw(SDL_Renderer* r)
{
	SDL_Color currentRenderColor = {0};
	SDL_GetRenderDrawColor(r, &currentRenderColor.r, &currentRenderColor.g,
	                       &currentRenderColor.b, &currentRenderColor.a);
	SDL_Color* desiredRenderColor = &default_colors.blocker;
	for(int y=0; y < GRIDHEIGHT; y++)
	{
		for(int x=0; x < GRIDWIDTH; x++)
		{
			switch(cell_buffer[x][y].material->id)
			{
			case CELL_BLOCKER:
				desiredRenderColor = &default_colors.blocker;
				break;
			case CELL_NOTHING:
				desiredRenderColor = &default_colors.air;
				break;
			case CELL_SAND:
				desiredRenderColor = &default_colors.sand;
				break;
			default:
				printf("ERROR: unreachable switch state\n");
				break;
			}
			
			if(currentRenderColor.r != desiredRenderColor->r ||
				currentRenderColor.g != desiredRenderColor->g ||
				currentRenderColor.b != desiredRenderColor->b)
			{
				memcpy(&currentRenderColor, desiredRenderColor, sizeof(SDL_Color));
				SDL_SetRenderDrawColor(r, currentRenderColor.r, currentRenderColor.g, currentRenderColor.b, currentRenderColor.a);
			}
			SDL_Rect rect = {x*CELLSIZE, SCREENHEIGHT - (y+1)*CELLSIZE, CELLSIZE, CELLSIZE};
			SDL_RenderFillRect(r, &rect);
		}
	}
	SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
	#ifdef DEBUG_DRAW_GRID

	for(int i=0; i < GRIDWIDTH; i++)
	{
		for(int j=0; j < GRIDHEIGHT; j++)
		{
			SDL_Rect rect = {i*CELLSIZE, j*CELLSIZE, CELLSIZE, CELLSIZE};
			SDL_RenderDrawRect(r, &rect);
		}
	}
	
	#endif
}
