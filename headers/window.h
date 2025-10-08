#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#define WINDOW_TITLE "Tangue"

#define FONT_FILE_PATH "assets/fonts/Orbitron/Orbitron-VariableFont_wght.ttf"
#define FONT_SIZE 24

typedef struct window_s
{
	int screen_width;
	int screen_height;
    SDL_Window *sdl_window;
    SDL_Renderer *sdl_renderer;
	TTF_Font *font;
} window_t;

int init_window(window_t **window);
int terminate_window(window_t *window);
int clear_window(window_t *window);
int present_window(window_t *window);
