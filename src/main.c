#include "log_utils.h"
#include "sdl_util.h"
#include "anna-layer.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <stdlib.h>
#include <time.h>
#include "cells.h"
#include "gfx.h"

#define TITLETEXT "powdertoy but worse"

#define TICK_INTERVAL 15

local u32 next_tick;

local u32 timeLeft()
{
	u32 now = SDL_GetTicks();
	if(next_tick <= now)
		return 0;
	else
		return next_tick - now;
}

int main()
{
	printf("individual cell size: %luB\n"
           "whole map size: ", sizeof(cell));
    log_human_readable_size(sizeof(cell)*GRIDHEIGHT*GRIDWIDTH);
	SDL_Window* window;
	SDL_Renderer* renderer;
	basicSetup(SCREENWIDTH, SCREENHEIGHT,
			   (SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER),
			   TITLETEXT, &window, &renderer);

	/* SDL_Surface* s = IMG_Load("res/player.png"); */
	/* SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s); */
	/* SDL_Rect testRect = {200,200,100,100}; */
	/* SDL_FreeSurface(s); */

	srand(time(NULL));

	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	/* memset(cell_buffer, CELL_AIR, sizeof(cell_t) * GRIDHEIGHT * GRIDWIDTH); */

	for(int i=0; i < GRIDWIDTH; i++)
	{
		for(int j=0; j < GRIDHEIGHT; j++)
		{
			cell_buffer[i][j].material = &cell_mats.nothing;
		}
	}

	SDL_Point mousePos = {0};
	SDL_Point gridPos = {-1};

	char running = 1;
	char placingBlock = 0;
	while(running)
	{
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_QUIT:
				running = 0;
				break;
			case SDL_KEYDOWN:
				switch(event.key.keysym.scancode)
				{
				case SDL_SCANCODE_ESCAPE:
					running = 0;
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEMOTION:
				mousePos.x = event.motion.x;
				mousePos.y = event.motion.y;
				if(mousePos.x >= 0 && mousePos.x < GRIDWIDTH*CELLSIZE &&
					mousePos.y >= 0 && mousePos.y < GRIDHEIGHT*CELLSIZE)
				{
					gridPos.x = mousePos.x / CELLSIZE;
					gridPos.y = (SCREENHEIGHT - mousePos.y) / CELLSIZE;
				}
				else
				{
					gridPos.x = -1;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if(event.button.button == SDL_BUTTON_LEFT)
				{
					placingBlock = 1;
				}
				else if (event.button.button == SDL_BUTTON_RIGHT)
				{
					placingBlock = 2;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if(event.button.button == SDL_BUTTON_LEFT)
				{
					placingBlock = 0;
				}
				else if (event.button.button == SDL_BUTTON_RIGHT)
				{
					placingBlock = 0;
				}
				break;
			}
		}

		if(gridPos.x != -1 && placingBlock)
			switch (placingBlock) {
			case 1:
				cell_buffer[gridPos.x][gridPos.y].material = &cell_mats.sand;
				break;
			case 2:
				cell_buffer[gridPos.x][gridPos.y].material = &cell_mats.water;
				break;
			}

		grid_update();

		// clear the back buffer
		SDL_RenderClear(renderer); 

		// copy texture to back buffer
		grid_draw(renderer);

		if(gridPos.x != -1)
		{
			SDL_Rect rect = {gridPos.x*CELLSIZE, SCREENHEIGHT - gridPos.y*CELLSIZE,
				CELLSIZE, CELLSIZE};
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderFillRect(renderer, &rect);
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderDrawPoint(renderer, mousePos.x, mousePos.y);
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		}

		// swap front and back buffers
		SDL_RenderPresent(renderer);

		// delay until the next frame
		SDL_Delay(timeLeft());
		next_tick += TICK_INTERVAL;
	}
}
