#include "sdl_util.h"
#include "anna-layer.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define SCREENWIDTH 1600
#define SCREENHEIGHT 900
#define TITLETEXT "window title"

#define TICK_INTERVAL 15

static u32 next_tick;

u32 timeLeft()
{
	u32 now = SDL_GetTicks();
	if(next_tick <= now)
		return 0;
	else
		return next_tick - now;
}

int main()
{
	SDL_Window* window;
	SDL_Renderer* renderer;
	basicSetup(SCREENWIDTH, SCREENHEIGHT,
			   (SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER),
			   TITLETEXT, &window, &renderer);

	SDL_Surface* s = IMG_Load("res/player.png");
	SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
	SDL_Rect testRect = {200,200,100,100};
	SDL_FreeSurface(s);
	
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	char running = 1;
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

			}
		}

		// clear the back buffer
		SDL_RenderClear(renderer); 

		// copy texture to back buffer
		SDL_RenderCopy(renderer, t, NULL, &testRect); 

		// swap front and back buffers
		SDL_RenderPresent(renderer);

		// delay until the next frame
		SDL_Delay(timeLeft());
		next_tick += TICK_INTERVAL;
	}
}
