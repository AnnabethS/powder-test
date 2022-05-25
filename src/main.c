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
#include "ui.h"

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

	if(ui_init() != 0)
	{
		fprintf(stderr, "error initialising UI, exiting...\n");
		return -1;
	}

	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	button_toggle_set button_set_material_select = {
		.length = 5,
		.selected = -1,
		.buttons = calloc(5, sizeof(button_toggle))};

	button_toggle_init_text(renderer, &button_set_material_select.buttons[0],
							"AIR", 1050, 50, 100, 50,
							button_color_palette.nothing_hovered,
							button_color_palette.unselected,
							button_color_palette.nothing_selected,
							NULL, NULL);
	button_toggle_init_text(renderer, &button_set_material_select.buttons[1],
							"SAND", 1150, 50, 100, 50,
							button_color_palette.sand_hovered,
							button_color_palette.unselected,
							button_color_palette.sand_selected,
							NULL, NULL);
	button_toggle_init_text(renderer, &button_set_material_select.buttons[2],
							"RED SAND", 1050, 100, 100, 50,
							button_color_palette.red_sand_hovered,
							button_color_palette.unselected,
							button_color_palette.red_sand_selected,
							NULL, NULL);
	button_toggle_init_text(renderer, &button_set_material_select.buttons[3],
							"WATER", 1150, 100, 100, 50,
							button_color_palette.water_hovered,
							button_color_palette.unselected,
							button_color_palette.water_selected,
							NULL, NULL);
	button_toggle_init_text(renderer, &button_set_material_select.buttons[4],
							"OIL", 1050, 150, 100, 50,
							button_color_palette.oil_hovered,
							button_color_palette.unselected,
							button_color_palette.oil_selected,
							NULL, NULL);
	
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
				if(mousePos.x < GRIDWIDTH*CELLSIZE)
				{
					if(event.button.button == SDL_BUTTON_LEFT)
					{
						placingBlock = 1;
					}
				}
				else
				{
					button_toggle_set_click(&button_set_material_select);
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if(event.button.button == SDL_BUTTON_LEFT)
				{
					placingBlock = 0;
				}
				break;
			}
		}

		if(gridPos.x != -1 && placingBlock)
			switch (button_set_material_select.selected) {
			case -1:
				break;
			case 0:
				cell_buffer[gridPos.x][gridPos.y].material = &cell_mats.nothing;
				break;
			case 1:
				cell_buffer[gridPos.x][gridPos.y].material = &cell_mats.sand;
				break;
			case 2:
				cell_buffer[gridPos.x][gridPos.y].material = &cell_mats.red_sand;
				break;
			case 3:
				cell_buffer[gridPos.x][gridPos.y].material = &cell_mats.water;
				break;
			case 4:
				cell_buffer[gridPos.x][gridPos.y].material = &cell_mats.oil;
				break;
			}

		grid_update();
		button_toggle_set_update(&button_set_material_select, mousePos.x, mousePos.y);

		// clear the back buffer
		SDL_RenderClear(renderer); 

		// copy texture to back buffer
		grid_draw(renderer);
		button_toggle_set_draw(renderer, &button_set_material_select);

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
