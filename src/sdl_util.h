#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

#ifndef ANNA_UTIL_SDL_HEADER
#define ANNA_UTIL_SDL_HEADER

/*
	*\param int windowWidth: desired width of the window
	*\param int windowHeight: desired height of the window
	*\param Uint32 flags: flags to init the renderer with
	*/
int basicSetup(int windowWidth, int windowHeight, Uint32 flags,
			   char* winTitle, SDL_Window** window, SDL_Renderer** renderer);


/* Safely Exits an SDL program
 */
int safeQuit(SDL_Window* window, SDL_Renderer* renderer);

#endif
