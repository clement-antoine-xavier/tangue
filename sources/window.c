#include "window.h"

#include <stdlib.h>
#include <stdio.h>

static int init_sdl(void)
{
	if (SDL_Init(SDL_INIT_VIDEO) != true)
	{
		fprintf(stderr, "[ERROR] [%s:%d] SDL_Init failed: %s\n", __FILE__, __LINE__, SDL_GetError());
		return EXIT_FAILURE;
	}

	if (TTF_Init() != true)
	{
		fprintf(stderr, "[ERROR] [%s:%d] TTF_Init failed: %s\n", __FILE__, __LINE__, SDL_GetError());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

static int get_screen_size(window_t *window)
{
	SDL_DisplayID display_id = 0;
	const SDL_DisplayMode *display_mode = NULL;

	display_id = SDL_GetPrimaryDisplay();
	if (display_id == 0)
	{
		fprintf(stderr, "[ERROR] [%s:%d] SDL_GetPrimaryDisplay failed: %s\n", __FILE__, __LINE__, SDL_GetError());
		return EXIT_FAILURE;
	}

	display_mode = SDL_GetCurrentDisplayMode(display_id);
	if (display_mode == NULL)
	{
		fprintf(stderr, "[ERROR] [%s:%d] SDL_GetCurrentDisplayMode failed: %s\n", __FILE__, __LINE__, SDL_GetError());
		return EXIT_FAILURE;
	}

	window->screen_width = display_mode->w;
	window->screen_height = display_mode->h;

	return EXIT_SUCCESS;
}

static int init_sdl_renderer(window_t *window)
{
	window->sdl_window = SDL_CreateWindow("Tangue", window->screen_width, window->screen_height, SDL_WINDOW_FULLSCREEN);
	if (window->sdl_window == NULL)
	{
		fprintf(stderr, "[ERROR] [%s:%d] SDL_CreateWindow failed: %s\n", __FILE__, __LINE__, SDL_GetError());
		return EXIT_FAILURE;
	}

	window->sdl_renderer = SDL_CreateRenderer(window->sdl_window, NULL);
	if (window->sdl_renderer == NULL)
	{
		fprintf(stderr, "[ERROR] [%s:%d] SDL_CreateRenderer failed: %s\n", __FILE__, __LINE__, SDL_GetError());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

static int create_font(window_t *window)
{
	window->font = TTF_OpenFont(FONT_FILE_PATH, FONT_SIZE);
	if (window->font == NULL)
	{
		fprintf(stderr, "[ERROR] [%s:%d] TTF_OpenFont failed: %s\n", __FILE__, __LINE__, SDL_GetError());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int init_window(window_t **window)
{
	if (init_sdl() != EXIT_SUCCESS)
	{
		fprintf(stderr, "[ERROR] [%s:%d] init_sdl failed in init_window\n", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}

	*window = (window_t *)malloc(sizeof(window_t));
	if (*window == NULL)
	{
		fprintf(stderr, "[ERROR] [%s:%d] Memory allocation failed in init_window\n", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}

	if (get_screen_size(*window) != EXIT_SUCCESS)
	{
		fprintf(stderr, "[ERROR] [%s:%d] get_screen_size failed in init_window\n", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}

	if (init_sdl_renderer(*window) != EXIT_SUCCESS)
	{
		fprintf(stderr, "[ERROR] [%s:%d] init_sdl_renderer failed in init_window\n", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}

	if (create_font(*window) != EXIT_SUCCESS)
	{
		fprintf(stderr, "[ERROR] [%s:%d] create_font failed in init_window\n", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

static void terminate_sdl()
{
	SDL_Quit();
	TTF_Quit();
}

static void terminate_sdl_renderer(window_t *window)
{
	SDL_DestroyRenderer(window->sdl_renderer);
	SDL_DestroyWindow(window->sdl_window);
}

static void terminate_font(window_t *window)
{
	TTF_CloseFont(window->font);
}

int terminate_window(window_t *window)
{
	terminate_font(window);
	terminate_sdl_renderer(window);
	free(window);
	terminate_sdl();
	return EXIT_SUCCESS;
}


int clear_window(window_t *window)
{
	if (SDL_SetRenderDrawColor(window->sdl_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE) != true)
	{
		fprintf(stderr, "[ERROR] [%s:%d] SDL_SetRenderDrawColor failed: %s\n", __FILE__, __LINE__, SDL_GetError());
		return EXIT_FAILURE;
	}

	if (SDL_RenderClear(window->sdl_renderer) != true)
	{
		fprintf(stderr, "[ERROR] [%s:%d] SDL_RenderClear failed: %s\n", __FILE__, __LINE__, SDL_GetError());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int present_window(window_t *window)
{
	if (SDL_RenderPresent(window->sdl_renderer) != true)
	{
		fprintf(stderr, "[ERROR] [%s:%d] SDL_RenderPresent failed: %s\n", __FILE__, __LINE__, SDL_GetError());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
