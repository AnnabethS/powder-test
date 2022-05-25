#include "gfx.h"
#include "anna-layer.h"
#include "cells.h"

struct colors color_palette = {
    .red.r = 255,
    .red.g = 0,
    .red.b = 0,
    .red.a = 255,

    .green.r = 0,
    .green.g = 255,
    .green.b = 0,
    .green.a = 255,

    .blue.r = 0,
    .blue.g = 0,
    .blue.b = 255,
    .blue.a = 255,

    .black.r = 0,
    .black.g = 0,
    .black.b = 0,
    .black.a = 255,

    .white.r = 255,
    .white.g = 255,
    .white.b = 255,
    .white.a = 255,

    .light_grey.r = 196,
    .light_grey.g = 196,
    .light_grey.b = 196,
    .light_grey.a = 255,

    .dark_grey.r = 100,
    .dark_grey.g = 100,
    .dark_grey.b = 100,
    .dark_grey.a = 255,
    };


struct button_color_defaults button_color_palette = {
	/* button_color unselected; */
	/* button_color nothing_selected; */
	/* button_color nothing_hovered; */
	/* button_color sand_selected; */
	/* button_color sand_hovered; */
	/* button_color red_sand_selected; */
	/* button_color red_sand_hovered; */
	/* button_color water_selected; */
	/* button_color water_hovered; */
	.unselected.bg = &color_palette.black,
	.unselected.border = &color_palette.light_grey,
	.unselected.text = &color_palette.light_grey,

	.nothing_hovered.bg = &color_palette.black,
	.nothing_hovered.border = &cell_mats.nothing.color,
	.nothing_hovered.text = &cell_mats.nothing.color,

	.nothing_selected.bg = &cell_mats.nothing.color,
	.nothing_selected.border = &cell_mats.nothing.color,
	.nothing_selected.text = &color_palette.black,

	.sand_hovered.bg = &color_palette.black,
	.sand_hovered.border = &cell_mats.sand.color,
	.sand_hovered.text = &cell_mats.sand.color,

	.sand_selected.bg = &cell_mats.sand.color,
	.sand_selected.border = &cell_mats.sand.color,
	.sand_selected.text = &color_palette.black,

	.red_sand_hovered.bg = &color_palette.black,
	.red_sand_hovered.border = &cell_mats.red_sand.color,
	.red_sand_hovered.text = &cell_mats.red_sand.color,

	.red_sand_selected.bg = &cell_mats.red_sand.color,
	.red_sand_selected.border = &cell_mats.red_sand.color,
	.red_sand_selected.text = &color_palette.black,

	.water_hovered.bg = &color_palette.black,
	.water_hovered.border = &cell_mats.water.color,
	.water_hovered.text = &cell_mats.water.color,

	.water_selected.bg = &cell_mats.water.color,
	.water_selected.border = &cell_mats.water.color,
	.water_selected.text = &color_palette.black,

	.oil_hovered.bg = &color_palette.black,
	.oil_hovered.border = &cell_mats.oil.color,
	.oil_hovered.text = &cell_mats.oil.color,

	.oil_selected.bg = &cell_mats.oil.color,
	.oil_selected.border = &cell_mats.oil.color,
	.oil_selected.text = &color_palette.black,
};
