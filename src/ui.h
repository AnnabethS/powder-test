#ifndef ANNA_UI_H
#define ANNA_UI_H

#include "cells.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>

#define BUTTON_FONT_PATH "res/NotoSansMono-Regular.ttf"

enum button_toggle_state {
	BUTTON_TOGGLE_UNHOVERED, // used when the button is unselected and the mouse is not over it
	BUTTON_TOGGLE_HOVERED,   // used when the button has the mouse over it
	BUTTON_TOGGLE_SELECTED   // used when the button is selected, regardless of mouse pos
};

enum button_push_state {
	BUTTON_PUSH_UNHOVERED, // used when the button is unselected and the mouse is not over it
	BUTTON_PUSH_HOVERED,   // used when the button has the mouse over it
	BUTTON_PUSH_PUSHED   // used when the button is actively being pushed
};

typedef struct{
	SDL_Color* bg;
	SDL_Color* border;
	SDL_Color* text;
}button_color;

typedef struct{
	SDL_Rect rect_box;
	SDL_Rect rect_text;
	SDL_Texture* text_texture_hovered;
	SDL_Texture* text_texture_unhovered;
	SDL_Texture* text_texture_selected;
	button_color color_hovered;
	button_color color_unhovered;
	button_color color_selected;
	enum button_toggle_state state;
	// points to a func that returns void and takes a void pointer
	void (*func_selected)(void*); 
	void* func_selected_param;
	char func_selected_enabled;
	// points to a func that returns void and takes a void pointer
	void (*func_deselected)(void*); 
	char func_deselected_enabled;
	void* func_deselected_param;
}button_toggle;

typedef struct{
	int length;
	int selected;
	button_toggle* buttons;
}button_toggle_set;

typedef struct{
	SDL_Rect rect_box;
	SDL_Rect rect_text;
	SDL_Texture* text_texture_unhovered;
	SDL_Texture* text_texture_hovered;
	SDL_Texture* text_texture_pushed;
	button_color color_unhovered;
	button_color color_hovered;
	button_color color_pushed;
	enum button_push_state state;
	void (*func_pushed)(void*); 
	void* func_pushed_param;
}button_push;

typedef struct{
	int length;
	button_push* buttons;
}button_push_set;

extern TTF_Font* button_font;
extern cell_material* cell_material_selected;

char ui_init();

char button_toggle_init_text(
	SDL_Renderer* r,
	button_toggle* dest,
	char* text,
	int x,
	int y,
	int w,
	int h,
	button_color hovered,
	button_color unhovered,
	button_color selected,
	void (*on_select_func)(void*), void* on_selected_param,
	void (*on_deselect_func)(void*), void* on_deselected_param);

/* void button_toggle_update(button_toggle* b, int mouse_x, int mouse_y); */

/* void button_toggle_draw(SDL_Renderer* r, button_toggle* b); */

void button_toggle_set_update(button_toggle_set* bs, int mouse_x, int mouse_y);

void button_toggle_set_draw(SDL_Renderer* r, button_toggle_set* bs);

void button_toggle_set_click(button_toggle_set* bs);

char button_push_init_text(
	SDL_Renderer* r,
	button_push* dest,
	char* text,
	int x,
	int y,
	int w,
	int h,
	button_color unhovered,
	button_color hovered,
	button_color pushed,
	void (*on_push_func)(void*), void* on_push_param);

void button_push_set_update(button_push_set* bs, int mouse_x, int mouse_y);

void button_push_set_draw(SDL_Renderer *r, button_push_set *bs);

void button_push_set_click(button_push_set* bs);
void button_push_set_unclick(button_push_set* bs);

void material_selected_set(void* new_material);

#endif
