#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

// Renderer configuration
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define FONT_SIZE 9
#define LARGE_FONT_SIZE 14
#define SMALL_FONT_SIZE 7
#define DISTANCE_DISPLAY_X (WINDOW_WIDTH - 200)
#define DISTANCE_DISPLAY_Y 20
#define TIMESTAMP_DISPLAY_X (WINDOW_WIDTH - 180)
#define TIMESTAMP_DISPLAY_Y 55

// Color definitions
typedef struct {
    SDL_Color white;
    SDL_Color green;
    SDL_Color red;
    SDL_Color yellow;
    SDL_Color cyan;
    SDL_Color orange;
    SDL_Color dark_gray;
    SDL_Color light_gray;
} color_palette_t;

// Renderer context structure
typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
    TTF_Font *large_font;
    TTF_Font *small_font;
    color_palette_t colors;
    int initialized;
} renderer_context_t;

// Function declarations
int renderer_init(renderer_context_t *ctx, const char *title);
void renderer_cleanup(renderer_context_t *ctx);
void renderer_clear(renderer_context_t *ctx);
void renderer_present(renderer_context_t *ctx);

// Drawing primitives
void draw_circle_outline(renderer_context_t *ctx, int center_x, int center_y, int radius, SDL_Color color);
void draw_thick_circle_outline(renderer_context_t *ctx, int center_x, int center_y, int radius, int thickness, SDL_Color color);
void draw_line_polar(renderer_context_t *ctx, int center_x, int center_y, int radius, float angle_deg, SDL_Color color);
void draw_arc_thick(renderer_context_t *ctx, int center_x, int center_y, int radius, float start_angle, float end_angle, int thickness, SDL_Color color);

// Text rendering
void render_text_with_font(renderer_context_t *ctx, TTF_Font *font_to_use, const char *text, int x, int y, SDL_Color color);
void render_text(renderer_context_t *ctx, const char *text, int x, int y, SDL_Color color);
void render_text_large(renderer_context_t *ctx, const char *text, int x, int y, SDL_Color color);
void render_text_small(renderer_context_t *ctx, const char *text, int x, int y, SDL_Color color);
