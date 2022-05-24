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

#define GRAVITY 2

// cell buffer that holds all of the cells, and is updated in place
cell cell_buffer[GRIDWIDTH][GRIDHEIGHT];
cell_materials cell_mats = {
    .blocker.id = CELL_BLOCKER,
    .blocker.solid = 1,
    .blocker.liquid = 0,
    .blocker.gas = 0,
    .blocker.flammable = 0,
    .blocker.finite_frames_to_live = 0,
    .blocker.color = {0,0,0,255},

    .nothing.id = CELL_NOTHING,
    .nothing.solid = 0,
    .nothing.liquid = 0,
    .nothing.gas = 0,
    .nothing.flammable = 0,
    .nothing.finite_frames_to_live = 0,
    .nothing.color = {197,245,255,255},

    .sand.id = CELL_SAND,
    .sand.solid = 1,
    .sand.liquid = 0,
    .sand.gas = 0,
    .sand.flammable = 0,
    .sand.finite_frames_to_live = 0,
    .sand.color = {255, 229, 187, 255},

    .red_sand.id = CELL_RED_SAND,
    .red_sand.solid = 1,
    .red_sand.liquid = 0,
    .red_sand.gas = 0,
    .red_sand.flammable = 1,
    .red_sand.finite_frames_to_live = 0,
    .red_sand.color = {255, 170, 109, 255},

    .water.id = CELL_WATER,
    .water.solid = 0,
    .water.liquid = 1,
    .water.liquid_spread_factor = 3,
    .water.gas = 0,
    .water.flammable = 0,
    .water.finite_frames_to_live = 0,
    .water.color = {50,102,255,255},

    .oil.id = CELL_OIL,
    .oil.solid = 0,
    .oil.liquid = 1,
    .oil.liquid_spread_factor = 1,
    .oil.gas = 0,
    .oil.flammable = 1,
    .oil.finite_frames_to_live = 0,
    .oil.color = {196, 180, 0, 255}
};


local cell_material* grid_get_cell_material(int x, int y)
{
	if (x < 0 || y < 0 || x >= GRIDWIDTH || y >= GRIDHEIGHT)
	{
		return &cell_mats.blocker;
	}

	return cell_buffer[x][y].material;
}

local void grid_swap_cell(int x1, int y1, int x2, int y2)
{
    cell tmp = cell_buffer[x2][y2];
    cell_buffer[x2][y2] = cell_buffer[x1][y1];
    cell_buffer[x1][y1] = tmp;
    cell_buffer[x1][y1].moved_this_frame = 1;
    cell_buffer[x2][y2].moved_this_frame = 1;
}

local void grid_move_cell(int from_x, int from_y, int to_x, int to_y)
{
    cell_buffer[to_x][to_y] = cell_buffer[from_x][from_y];
    cell_buffer[from_x][from_y].material = &cell_mats.nothing;
    cell_buffer[from_x][from_y].moved_this_frame = 1;
    cell_buffer[to_x][to_y].moved_this_frame = 1;
}

local char is_liquid_blocking(cell_material* moving, cell_material* target)
{
    if(target->id == CELL_NOTHING)
        return 0;
    if(target->solid)
        return 1;
    if(target->gas)
        return 0;
    return moving->id <= target->id;
}

local void grid_solid_update(int x, int y)
{
    // should be 1 to choose right and -1 to choose left, inverted after
    // each choice. this eliminates pseduorandom number generation from
    // the movement, and makes movements deterministic
    persist uint choose_side = 1;
	/*
	solid CA update options 

	Legend:
	. = AIR
	X = SOLID
	? = DONT CARE
	*/

    /*
      GRID FORMAT:
      1 2 3
      4 5 6
      7 8 9
     */
    if(cell_buffer[x][y].moved_this_frame)
        return;

    cell_material *c1, *c2, *c3, *c4, *c5, *c6, *c7, *c8, *c9;
    c1 = grid_get_cell_material(x-1, y+1);
    c2 = grid_get_cell_material(x, y+1);
    c3 = grid_get_cell_material(x+1, y+1);

    c4 = grid_get_cell_material(x-1, y);
    c5 = grid_get_cell_material(x, y);
    c6 = grid_get_cell_material(x+1, y);

    c7 = grid_get_cell_material(x-1, y-1);
    c8 = grid_get_cell_material(x, y-1);
    c9 = grid_get_cell_material(x+1, y-1);

    /*
    ???    ???
    ?X? -> ?X?
    XXX    XXX
    */
    if(c7->solid && c8->solid && c9->solid)
    {
        return;
    }

    /*
    ???    ???
    ?X. -> ?..
    XX.    XXX
    */
    if(c7->solid && c8->solid && !c9->solid && !c6->solid)
    { // we can move there
        if(c9->liquid || c9->gas)
        { // swap
            grid_swap_cell(x, y, x+1, y-1);
        }
        else
        { // just move
            grid_move_cell(x, y, x+1, y-1);
        }
        return;
    }

    /*
    ???    ???
    .X? -> ..?
    .XX    XXX
    */
    if(c9->solid && c8->solid && !c7->solid && !c4->solid)
    { // we can move there
        if(c7->liquid || c7->gas)
        { // swap
            grid_swap_cell(x, y, x-1, y-1);
        }
        else
        { // just move
            grid_move_cell(x, y, x-1, y-1);
        }
        return;
    }

    /*
    ???    ???    ???
    .X. -> ... or ...
    .X.    XX.    .XX
    */

    if(c8->solid && !c7->solid && !c9->solid && !c4->solid && !c6->solid)
    {
        cell_material* chosen = grid_get_cell_material(x+choose_side, y-1);
        if(chosen->gas || chosen->liquid)
        {
            grid_swap_cell(x, y, x+choose_side, y-1);
        }
        else
        {
            grid_move_cell(x, y, x+choose_side, y-1);
        }
        choose_side *= -1;
        return;
    }

    /*
    ???    ???
    ?X? -> ?.?
    ?.?    ?X?
    */
    if(!c8->solid)
    {
        if(c8->liquid || c8->gas)
        {
            grid_swap_cell(x, y, x, y-1);
            return;
        }
        for(int i=2; i <= GRAVITY; i++)
        {
            cell_material* check = grid_get_cell_material(x, y-i);
            if(check->solid)
            {
                grid_move_cell(x, y, x, y-(i-1));
                return;
            }
            else if(check->liquid || check->gas)
            {
                grid_move_cell(x, y-i, x, (y-i)+1);
                grid_move_cell(x, y, x, y-i);
                return;
            }
        }
        grid_move_cell(x, y, x, y-GRAVITY);

        return;
    }
}

local void grid_liquid_update(int x, int y)
{
    // should be 1 to choose right and -1 to choose left, inverted after
    // each choice. this eliminates pseduorandom number generation from
    // the movement, and makes movements deterministic
    persist uint choose_side = 1;

    if(cell_buffer[x][y].moved_this_frame)
        return;

    cell_material *c1, *c2, *c3, *c4, *c5, *c6, *c7, *c8, *c9;
    c1 = grid_get_cell_material(x-1, y+1);
    c2 = grid_get_cell_material(x, y+1);
    c3 = grid_get_cell_material(x+1, y+1);

    c4 = grid_get_cell_material(x-1, y);
    c5 = grid_get_cell_material(x, y);
    c6 = grid_get_cell_material(x+1, y);

    c7 = grid_get_cell_material(x-1, y-1);
    c8 = grid_get_cell_material(x, y-1);
    c9 = grid_get_cell_material(x+1, y-1);

    /*
    ???    ???
    ?X? -> ?X?
    XXX    XXX
    */
    //if(c7->solid && c8->solid && c9->solid)
    /* if(is_liquid_blocking(c5, c7) && */
    /*     is_liquid_blocking(c5, c8) && */
    /*     is_liquid_blocking(c5, c9)) */
    /* { */
    /*     return; */
    /* } */

    /*
    ???    ???
    ?X. -> ?..
    XX.    XXX
    */
    //if(c7->solid && c8->solid && !c9->solid && !c6->solid)
    if(is_liquid_blocking(c5, c7) &&
        is_liquid_blocking(c5, c8) &&
       (!is_liquid_blocking(c5, c9)) &&
       (!is_liquid_blocking(c5, c6)))
    { // we can move there
        if (!(c9->liquid || c9->gas)) // its nothing, just move
        {
            grid_move_cell(x, y, x+1, y-1);
        }
        else
        { 
            grid_move_cell(x+1, y-1, x+1, y);
            grid_move_cell(x, y, x+1, y-1);
        }
        return;
    }

    /*
    ???    ???
    .X? -> ..?
    .XX    XXX
    */
    //if(c9->solid && c8->solid && !c7->solid && !c4->solid)
    if(is_liquid_blocking(c5, c9) &&
        is_liquid_blocking(c5, c8) &&
       (!is_liquid_blocking(c5, c7)) &&
       (!is_liquid_blocking(c5, c4)))
    { // we can move there

        /* if(c7->liquid || c7->gas) */
        /* { // swap */
        /*     grid_swap_cell(x, y, x-1, y-1); */
        /* } */
        /* else */
        /* { // just move */
        /*     grid_move_cell(x, y, x-1, y-1); */
        /* } */

        if (!(c7->liquid || c7->gas)) // its nothing, just move
        {
            grid_move_cell(x, y, x-1, y-1);
        }
        else
        { 
            grid_move_cell(x-1, y-1, x-1, y);
            grid_move_cell(x, y, x-1, y-1);
        }
        return;
    }

    /*
    ???    ???    ???
    .X. -> ... or ...
    .X.    XX.    .XX
    */

    //if(c8->solid && !c7->solid && !c9->solid && !c4->solid && !c6->solid)
    if(is_liquid_blocking(c5, c8) &&
       (!is_liquid_blocking(c5, c7)) &&
       (!is_liquid_blocking(c5, c9)) &&
       (!is_liquid_blocking(c5, c4)) &&
       (!is_liquid_blocking(c5, c6)))
    {
        cell_material* chosen = grid_get_cell_material(x+choose_side, y-1);
        /* if(chosen->gas || chosen->liquid) */
        /* { */
        /*     grid_swap_cell(x, y, x+choose_side, y-1); */
        /* } */
        /* else */
        /* { */
        /*     grid_move_cell(x, y, x+choose_side, y-1); */
        /* } */

        if (!(chosen->liquid || chosen->gas)) // its nothing, just move
        {
            grid_move_cell(x, y, x+choose_side, y-1);
        }
        else 
        { 
            grid_move_cell(x+choose_side, y-1, x+choose_side, y);
            grid_move_cell(x, y, x+choose_side, y-1);
        }
        
        choose_side *= -1;
        return;
    }

    /*
    ???    ???
    ?X? -> ?.?
    ?.?    ?X?
    */
    if(!is_liquid_blocking(c5, c8))
    {
        if(c8->gas)
        {
            grid_swap_cell(x, y, x, y-1);
            return;
        }
        else if(c8->liquid && c8->id < c5->id)
        {
            grid_swap_cell(x, y, x, y-1);
            return;
        }
        else if (c8->id == c5->id)
            return;
        for(int i=2; i <= GRAVITY; i++)
        {
            cell_material* check = grid_get_cell_material(x, y-i);
            if(check->solid)
            {
                grid_move_cell(x, y, x, (y-i)+1);
                return;
            }
            else if(check->gas)
            {
                grid_swap_cell(x, y, x, y-(i-1));
                return;
            }
            else if(check->liquid)
            {
                if(check->id < c5->id)
                {
                    grid_move_cell(x, y-i, x, (y-i)+1);
                    grid_move_cell(x, y, x, y-i);
                }
                else
                {
                    grid_move_cell(x, y, x, (y-i)+1);
                }
                return;
            }
        }
        grid_move_cell(x, y, x, y-GRAVITY);
        return;
    }

    /* FLUID ONLY SPREAD*/
    /*
      ???    ???
      .XS -> X.S
      SSS    SSS
     */
    if((!(c4->solid || c4->liquid || c4->gas)) && (c6->liquid || c6->solid || c6->gas))
    {
        // spread factor
        for(int i=2; i <= c5->liquid_spread_factor; i++)
        {
            cell_material* check = grid_get_cell_material(x-i, y);
            if(check->solid || check->liquid || check->gas)
            {
                grid_move_cell(x, y, (x-i)+1, y);
                return;
            }
        }
        grid_move_cell(x, y, x - c5->liquid_spread_factor, y);
        return;
    }

    /*
      ???    ???
      SX. -> S.X
      SSS    SSS
     */

    if(((c4->solid || c4->liquid || c4->gas)) && (!(c6->liquid || c6->solid || c6->gas)))
    {
        // spread factor
        for(int i=2; i <= c5->liquid_spread_factor; i++)
        {
            cell_material* check = grid_get_cell_material(x+i, y);
            if(check->solid || check->liquid || check->gas)
            {
                grid_move_cell(x, y, (x+i)-1, y);
                return;
            }
        }
        grid_move_cell(x, y, x + c5->liquid_spread_factor, y);
        return;
    }

    /*
      ???    ???    ???
      .X. -> ..X or X..
      SSS    SSS    SSS
     */
    if(!(c4->solid || c4->liquid || c4->gas || c6->solid || c6->liquid || c6->gas))
    {
        if(choose_side == -1)
        {
            for(int i=2; i <= c5->liquid_spread_factor; i++)
            {
                cell_material* check = grid_get_cell_material(x-i, y);
                if(check->solid || check->liquid || check->gas)
                {
                    grid_move_cell(x, y, (x-i)+1, y);
                    choose_side *= -1;
                    return;
                }
            }
            grid_move_cell(x, y, x - c5->liquid_spread_factor, y);
        }
        else
        {
            for(int i=2; i <= c5->liquid_spread_factor; i++)
            {
                cell_material* check = grid_get_cell_material(x+i, y);
                if(check->solid || check->liquid || check->gas)
                {
                    grid_move_cell(x, y, (x+i)-1, y);
                    choose_side *= -1;
                    return;
                }
            }
            grid_move_cell(x, y, x + c5->liquid_spread_factor, y);
        }
        choose_side *= -1;
        return;
    }
}

local void grid_gas_update(int x, int y)
{
    
}

void grid_update()
{
	for(int x=0; x < GRIDWIDTH; x++)
	{
		for(int y=0; y < GRIDHEIGHT; y++)
		{
            if(cell_buffer[x][y].material->id != CELL_BLOCKER &&
               cell_buffer[x][y].material->id != CELL_NOTHING)
            {
                if(cell_buffer[x][y].material->solid)
                    grid_solid_update(x,y);
                else if(cell_buffer[x][y].material->liquid)
                    grid_liquid_update(x, y);
            }
		}
	}

    // a potential optimisation is to store this info in another buffer, and
    // use memset to reset it, which may be faster due to optimisation
	for(int x=0; x < GRIDWIDTH; x++)
	{
		for(int y=0; y < GRIDHEIGHT; y++)
		{
            cell_buffer[x][y].moved_this_frame = 0;
		}
	}
}

void grid_draw(SDL_Renderer* r)
{
	SDL_Color currentRenderColor = {0};
	SDL_GetRenderDrawColor(r, &currentRenderColor.r, &currentRenderColor.g,
	                       &currentRenderColor.b, &currentRenderColor.a);
	SDL_Color* desiredRenderColor = {0};
	for(int y=0; y < GRIDHEIGHT; y++)
	{
		for(int x=0; x < GRIDWIDTH; x++)
		{
            desiredRenderColor = &cell_buffer[x][y].material->color;
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
