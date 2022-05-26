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

void game_quit(void* nothing);

char game_running = 1;

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

    int circle_rad = 3;

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
							&material_selected_set, &cell_mats.nothing,
							NULL, NULL);
	button_toggle_init_text(renderer, &button_set_material_select.buttons[1],
							"SAND", 1150, 50, 100, 50,
							button_color_palette.sand_hovered,
							button_color_palette.unselected,
							button_color_palette.sand_selected,
							&material_selected_set, &cell_mats.sand,
							NULL, NULL);
	button_toggle_init_text(renderer, &button_set_material_select.buttons[2],
							"RED SAND", 1050, 100, 100, 50,
							button_color_palette.red_sand_hovered,
							button_color_palette.unselected,
							button_color_palette.red_sand_selected,
							&material_selected_set, &cell_mats.red_sand,
							NULL, NULL);
	button_toggle_init_text(renderer, &button_set_material_select.buttons[3],
							"WATER", 1150, 100, 100, 50,
							button_color_palette.water_hovered,
							button_color_palette.unselected,
							button_color_palette.water_selected,
							&material_selected_set, &cell_mats.water,
							NULL, NULL);
	button_toggle_init_text(renderer, &button_set_material_select.buttons[4],
							"OIL", 1050, 150, 100, 50,
							button_color_palette.oil_hovered,
							button_color_palette.unselected,
							button_color_palette.oil_selected,
							&material_selected_set, &cell_mats.oil,
							NULL, NULL);
	
	/* memset(cell_buffer, CELL_AIR, sizeof(cell_t) * GRIDHEIGHT * GRIDWIDTH); */

	button_push_set button_set_other = {
		.length = 2,
		.buttons = calloc(2, sizeof(button_push))};

	button_push_init_text(renderer, &button_set_other.buttons[0],
						  "clear grid", 1050, 800, 100, 50,
						  button_color_palette.unselected,
						  button_color_palette.grid_clear_hovered,
						  button_color_palette.grid_clear_pushed,
						  &grid_clear, NULL);


	button_push_init_text(renderer, &button_set_other.buttons[1],
						  "quit game", 1150, 800, 100, 50,
						  button_color_palette.unselected,
						  button_color_palette.game_quit_hovered,
						  button_color_palette.game_quit_pushed,
						  &game_quit, NULL);
	
	for(int i=0; i < GRIDWIDTH; i++)
	{
		for(int j=0; j < GRIDHEIGHT; j++)
		{
			cell_buffer[i][j].material = &cell_mats.nothing;
		}
	}

	SDL_Point mousePos = {0};
	SDL_Point gridPos = {-1};

	char placingBlock = 0;
	while(game_running)
	{
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_QUIT:
				game_quit(NULL);
				break;
			case SDL_KEYDOWN:
				switch(event.key.keysym.scancode)
				{
				case SDL_SCANCODE_ESCAPE:
					game_quit(NULL);
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
					button_push_set_click(&button_set_other);
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if(event.button.button == SDL_BUTTON_LEFT)
				{
					placingBlock = 0;
					if(mousePos.x > GRIDWIDTH*CELLSIZE)
					{
						button_push_set_unclick(&button_set_other);
					}
				}
				break;
			}
		}

		if(gridPos.x != -1 && placingBlock)
        {
            if (cell_material_selected != NULL)
            {
                int top = floor((double)gridPos.y - ((double)circle_rad + 0.5));
                int bot = ceil((double)gridPos.y + ((double)circle_rad + 0.5));
                int left = floor((double)gridPos.x - ((double)circle_rad + 0.5));
                int right = ceil((double)gridPos.x + ((double)circle_rad + 0.5));

                if (top < 0)
                    top = 0;

                if (left < 0)
                    left = 0;
                
                if (bot >= GRIDHEIGHT)
                    bot = GRIDHEIGHT;

                if (right >= GRIDWIDTH)
                    right = GRIDWIDTH;

                for (int y = top; y <= bot; y++)
                {
                    for (int x = left; x <= right; x++)
                    {
                        if ((pow(x - gridPos.x, 2) + pow(y - gridPos.y, 2) <=
                           pow((double)circle_rad + 0.5, 2)))
                        {
                            cell_buffer[x][y].material = cell_material_selected;
                        }                        
                    }                    
                }                  
            }
        }

		grid_update();
		button_toggle_set_update(&button_set_material_select, mousePos.x, mousePos.y);
		button_push_set_update(&button_set_other, mousePos.x, mousePos.y);

		// clear the back buffer
		SDL_RenderClear(renderer); 

		// copy texture to back buffer
		grid_draw(renderer);
		button_toggle_set_draw(renderer, &button_set_material_select);
		button_push_set_draw(renderer, &button_set_other);

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

// must have this function signature to use the function pointer
void game_quit(void* nothing)
{
	game_running = 0;
}
