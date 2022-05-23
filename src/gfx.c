#include "gfx.h"
#include "anna-layer.h"

struct colors_t default_colors;

//alpha value is implicitly set to 255
local void setDefaultColor(SDL_Color* c, u8 r, u8 g, u8 b)
{
	c->r = r;
	c->g = g;
	c->b = b;
	c->a = 255;
}

void GFXSetDefaults()
{
	setDefaultColor(&default_colors.blocker, 0, 0, 0);
	setDefaultColor(&default_colors.air, 128, 128, 128);
	setDefaultColor(&default_colors.sand, 203, 207, 175);
	setDefaultColor(&default_colors.water, 90, 90, 255);
}
