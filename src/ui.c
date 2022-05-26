#include "ui.h"
#include "cells.h"

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

TTF_Font* button_font;
cell_material* cell_material_selected = NULL;

char ui_init()
{
	if(TTF_Init() != 0)
	{
		fprintf(stderr, "could not init TTF\n");
		return -1;
	}
	button_font = TTF_OpenFont(BUTTON_FONT_PATH, 12);
	if(button_font == NULL)
	{
		fprintf(stderr, "could not load font: %s\n", BUTTON_FONT_PATH);
		return -1;
	}
	return 0;
}

char button_toggle_init_text(
	SDL_Renderer* r,
	button_toggle* dest,
	char* text,
	int x, int y, int w, int h,
	button_color hovered,
	button_color unhovered,
	button_color selected,
	void (*on_select_func)(void*), void* on_selected_param,
	void (*on_deselect_func)(void*), void* on_deselected_param)
{
	dest->rect_box.x = x;
	dest->rect_box.y = y;
	dest->rect_box.w = w;
	dest->rect_box.h = h;

	dest->color_hovered = hovered;
	dest->color_unhovered = unhovered;
	dest->color_selected = selected;

	dest->func_selected = on_select_func;
    dest->func_selected_enabled = (on_select_func != NULL);
    dest->func_selected_param = on_selected_param;
	dest->func_deselected = on_deselect_func;
    dest->func_deselected_enabled = (on_deselect_func != NULL);
    dest->func_deselected_param = on_deselected_param;

	SDL_Surface* s = TTF_RenderText_Solid(button_font, text, *hovered.text);
	if(s == NULL)
	{
		fprintf(stderr, "could not load button with text: %s\n", text);
		return -1;
	}
	dest->text_texture_hovered = SDL_CreateTextureFromSurface(r, s);
	if(dest->text_texture_hovered == NULL)
	{
		fprintf(stderr, "could not load button with text: %s\n", text);
		return -1;
	}
	SDL_FreeSurface(s);

	s = TTF_RenderText_Solid(button_font, text, *unhovered.text);
	if(s == NULL)
	{
		fprintf(stderr, "could not load button with text: %s\n", text);
		return -1;
	}
	dest->text_texture_unhovered = SDL_CreateTextureFromSurface(r, s);
	if(dest->text_texture_unhovered == NULL)
	{
		fprintf(stderr, "could not load button with text: %s\n", text);
		return -1;
	}
	SDL_FreeSurface(s);

	s = TTF_RenderText_Solid(button_font, text, *selected.text);
	if(s == NULL)
	{
		fprintf(stderr, "could not load button with text: %s\n", text);
		return -1;
	}
	dest->text_texture_selected = SDL_CreateTextureFromSurface(r, s);
	if(dest->text_texture_selected == NULL)
	{
		fprintf(stderr, "could not load button with text: %s\n", text);
		return -1;
	}
	SDL_FreeSurface(s);
	

	SDL_QueryTexture(dest->text_texture_hovered, NULL, NULL,
					 &dest->rect_text.w, &dest->rect_text.h);
	dest->rect_text.x = dest->rect_box.x +
		(dest->rect_box.w/2) - (dest->rect_text.w/2);
	dest->rect_text.y = dest->rect_box.y +
		(dest->rect_box.h/2) - (dest->rect_text.h/2);

	dest->state = BUTTON_TOGGLE_UNHOVERED;

	return 0;
}

void button_toggle_update(button_toggle* b, int mouse_x, int mouse_y)
{
	if(b->state != BUTTON_TOGGLE_SELECTED)
	{
		if(mouse_x >= b->rect_box.x && mouse_x <= b->rect_box.x + b->rect_box.w &&
			mouse_y >= b->rect_box.y && mouse_y <= b->rect_box.y + b->rect_box.h)
		{
			b->state = BUTTON_TOGGLE_HOVERED;
		}
		else
		{
			b->state = BUTTON_TOGGLE_UNHOVERED;
		}
	}
}

void button_toggle_draw(SDL_Renderer* r, button_toggle* b)
{
	button_color* color;
	SDL_Texture* text_texture;
	switch (b->state)
	{
	case BUTTON_TOGGLE_UNHOVERED:
		color = &b->color_unhovered;
		text_texture = b->text_texture_unhovered;
		break;
	case BUTTON_TOGGLE_HOVERED:
		color = &b->color_hovered;
		text_texture = b->text_texture_hovered;
		break;
	case BUTTON_TOGGLE_SELECTED:
		color = &b->color_selected;
		text_texture = b->text_texture_selected;
		break;
	}
	SDL_SetRenderDrawColor(r, color->bg->r, color->bg->g, color->bg->b, color->bg->a);
	SDL_RenderFillRect(r, &b->rect_box);
	SDL_SetRenderDrawColor(r, color->border->r, color->border->g, color->border->b, color->border->a);
	SDL_RenderDrawRect(r, &b->rect_box);
	SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
	SDL_RenderCopy(r, text_texture, NULL, &b->rect_text);
}

void button_toggle_set_update(button_toggle_set* bs, int mouse_x, int mouse_y)
{
	for(int i=0; i < bs->length; i++)
	{
		button_toggle_update(&bs->buttons[i], mouse_x, mouse_y);
	}
}

void button_toggle_set_draw(SDL_Renderer* r, button_toggle_set* bs)
{
	for(int i=0; i < bs->length; i++)
	{
		button_toggle_draw(r, &bs->buttons[i]);
	}
}

void button_toggle_set_click(button_toggle_set* bs)
{
	for(int i=0; i < bs->length; i++)
	{
		if(bs->buttons[i].state == BUTTON_TOGGLE_HOVERED)
		{
			bs->buttons[i].state = BUTTON_TOGGLE_SELECTED;

			if(bs->buttons[i].func_selected_enabled)
            {
				bs->buttons[i].func_selected(bs->buttons[i].func_selected_param);
            }

			bs->buttons[bs->selected].state = BUTTON_TOGGLE_UNHOVERED;

            if(bs->selected != -1)
                if(bs->buttons[bs->selected].func_deselected_enabled)
                    bs->buttons[bs->selected].func_deselected(bs->buttons[bs->selected].func_deselected_param);

			bs->selected = i;
			break;
		}
	}
}

void material_selected_set(void* new_material)
{
    cell_material_selected = (cell_material*) new_material;
}
