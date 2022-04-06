#include "cells.h"
#include "anna-layer.h"
#include "gfx.h"
#include <SDL2/SDL_render.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string.h>

//#define DEBUG_DRAW_GRID

// cell buffer that holds all of the cells, and is updated in place
cell_t cell_buffer[GRIDWIDTH][GRIDHEIGHT];

local char cellIsSolid(cell_t type)
{
	return type == CELL_BLOCKER || type == CELL_SAND;
}

local cell_t gridGetCell(int x, int y)
{
	if (x < 0 || y < 0 || x >= GRIDWIDTH || y >= GRIDHEIGHT)
	{
		return CELL_BLOCKER;
	}

	return cell_buffer[x][y];
}

void cellSandUpdate(int x, int y)
{
	/*
	SAND CA update options 

	Legend:
	. = AIR
	X = SOLID
	? = DONT CARE
	*/

	cell_t c00, c01, c02, c10, c11, c12;
	c11 = gridGetCell(x, y);
	if (c11 != CELL_SAND)
	{
		printf("ERROR: sand updater passed a cell that is not sand\n");
	}
	
	
	//cXY relative to x and y params
	c00 = gridGetCell(x-1, y-1);
	c01 = gridGetCell(x, y-1);
	c02 = gridGetCell(x+1, y-1);
	c10 = gridGetCell(x-1, y);
	c12 = gridGetCell(x+1, y);
	
	if (cellIsSolid(c00) && cellIsSolid(c01) && cellIsSolid(c02))
	{
		/*
		???    ???
		?X? -> ?X?
		XXX    XXX
		*/
		return;
	}
	else if(cellIsSolid(c00) && cellIsSolid(c01) && c02 == CELL_AIR && c10 == CELL_AIR && c12 == CELL_AIR)
	{
		/*
		???    ???
		.X. -> ...
		XX.    XXX
		*/
		cell_buffer[x][y] = CELL_AIR;
		cell_buffer[x+1][y-1] = CELL_SAND;

	}
	else if(cellIsSolid(c01) && cellIsSolid(c02) && c00 == CELL_AIR && c10 == CELL_AIR && c12 == CELL_AIR)
	{
		/*
		???    ???
		.X. -> ...
		.XX    XXX
		*/
		cell_buffer[x][y] = CELL_AIR;
		cell_buffer[x-1][y-1] = CELL_SAND;
	}
	else if(cellIsSolid(c01) && c00 == CELL_AIR && c10 == CELL_AIR && c02 == CELL_AIR && c12 == CELL_AIR)
	{
		/*
		???    ???    ???
		.X. -> ... or ...
		.X.    XX.    .XX
		*/
		if(rand() % 2 == 0)
		{ // pick left
			cell_buffer[x][y] = CELL_AIR;
			cell_buffer[x-1][y-1] = CELL_SAND;
		}
		else
		{ // pick right
			cell_buffer[x][y] = CELL_AIR;
			cell_buffer[x+1][y-1] = CELL_SAND;
		}
	}
	else if(c01 == CELL_AIR)
	{
		/*
		???    ???
		?X? -> ?.?
		?.?    ?X?
		*/
		cell_buffer[x][y] = CELL_AIR;
		cell_buffer[x][y-1] = CELL_SAND;
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
			switch(cell_buffer[x][y])
			{
			case CELL_BLOCKER:
				desiredRenderColor = &default_colors.blocker;
				break;
			case CELL_AIR:
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
			SDL_Rect rect = {x*CELLSIZE, y*CELLSIZE, CELLSIZE, CELLSIZE};
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
