#include <stdio.h>
#include <SDL2/SDL.h>
#include "sdl_util.h"

/*
	*\param int windowWidth: desired width of the window
	*\param int windowHeight: desired height of the window
	*\param Uint32 flags: flags to init the renderer with
	*/
int basicSetup(int windowWidth, int windowHeight, Uint32 flags, char *winTitle,
			   SDL_Window **window_ptr_ptr, SDL_Renderer **renderer_ptr_ptr)
{
	//attempt to init SDL with correct flags
	if (SDL_Init(flags) != 0)
	{
		printf("Error Occured Initialising SDL: %s\n", SDL_GetError());
		return 1;
	}

	//init window at the pointer provided
	*window_ptr_ptr = SDL_CreateWindow(winTitle,
					SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
					windowWidth, windowHeight, 0);
	if(!*window_ptr_ptr)
	{
		printf("Error Occured Creating Window: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	//create a renderer at the pointer provided
	Uint32 render_flags = SDL_RENDERER_ACCELERATED;
	*renderer_ptr_ptr = SDL_CreateRenderer(*window_ptr_ptr, -1, render_flags);
	if(!*renderer_ptr_ptr)
	{
		printf("Error Occured Creating Renderer: %s\n", SDL_GetError());
		SDL_DestroyWindow(*window_ptr_ptr);
		SDL_Quit();
		return 1;
	}
	return 0;
}

int safeQuit(SDL_Window* window, SDL_Renderer* renderer)
{
	if(renderer != NULL)
		SDL_DestroyRenderer(renderer);
	if(window != NULL)
		SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
}
