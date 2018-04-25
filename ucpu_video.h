#ifndef UCPU_VIDEO_H
#define UCPU_VIDEO_H

#include "ucpu_types.h"

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#define UCPU_VIDEO_WIDTH 120
#define UCPU_VIDEO_HEIGHT 96
#define UCPU_WINDOW_UPSCALE 4

enum uCPUColor {
	uCPUColor_Bright = 0,
	uCPUColor_Shade1,
	uCPUColor_Shade2,
	uCPUColor_Dark,
	uCPUColor_Ignore,
};

typedef struct uColor_t {
	u8 r, g, b;
} uColor;

extern uColor uCPU_Palette[];

typedef struct uGfx_t {
	uMem *vram;
	
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *screen_texture;
} uGfx;

uGfx* ugfx_new();
void ugfx_free(uGfx* gfx);

void ugfx_clear(uGfx* gfx, u8 color);

void ugfx_flip(uGfx* gfx);

#endif /* UCPU_VIDEO_H */

