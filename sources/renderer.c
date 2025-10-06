#include "renderer.h"
#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static void init_color_palette(color_palette_t *colors) {
    colors->white = (SDL_Color){255, 255, 255, 255};
    colors->green = (SDL_Color){0, 255, 0, 255};
    colors->red = (SDL_Color){255, 0, 0, 255};
    colors->yellow = (SDL_Color){255, 255, 0, 255};
    colors->cyan = (SDL_Color){0, 255, 255, 255};
    colors->orange = (SDL_Color){255, 165, 0, 255};
    colors->dark_gray = (SDL_Color){40, 40, 40, 255};
    colors->light_gray = (SDL_Color){120, 120, 120, 255};
}

int renderer_init(renderer_context_t *ctx, const char *title) {
    // Initialize context
    ctx->window = NULL;
    ctx->renderer = NULL;
    ctx->font = NULL;
    ctx->large_font = NULL;
    ctx->small_font = NULL;
    ctx->initialized = 0;

    // Initialize color palette
    init_color_palette(&ctx->colors);

    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return -1;
    }

    // Initialize SDL_ttf
    if (!TTF_Init()) {
        SDL_Quit();
        return -1;
    }

    // Create window
    ctx->window = SDL_CreateWindow(title, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (!ctx->window) {
        renderer_cleanup(ctx);
        return -1;
    }

    // Create renderer
    ctx->renderer = SDL_CreateRenderer(ctx->window, NULL);
    if (!ctx->renderer) {
        renderer_cleanup(ctx);
        return -1;
    }

    // Load fonts
    const char *font_paths[] = {
        "./assets/fonts/Orbitron/Orbitron-VariableFont_wght.ttf",
        "/System/Library/Fonts/Menlo.ttc",
        "/System/Library/Fonts/Monaco.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
        NULL
    };

    const char *loaded_font_path = NULL;
    for (int i = 0; font_paths[i] != NULL; i++) {
        ctx->font = TTF_OpenFont(font_paths[i], FONT_SIZE);
        if (ctx->font) {
            loaded_font_path = font_paths[i];
            break;
        }
    }

    if (!ctx->font) {
        renderer_cleanup(ctx);
        return -1;
    }

    // Load additional font sizes
    ctx->large_font = TTF_OpenFont(loaded_font_path, LARGE_FONT_SIZE);
    ctx->small_font = TTF_OpenFont(loaded_font_path, SMALL_FONT_SIZE);

    ctx->initialized = 1;
    return 0;
}

void renderer_cleanup(renderer_context_t *ctx) {
    if (ctx->small_font) {
        TTF_CloseFont(ctx->small_font);
        ctx->small_font = NULL;
    }
    if (ctx->large_font) {
        TTF_CloseFont(ctx->large_font);
        ctx->large_font = NULL;
    }
    if (ctx->font) {
        TTF_CloseFont(ctx->font);
        ctx->font = NULL;
    }
    if (ctx->renderer) {
        SDL_DestroyRenderer(ctx->renderer);
        ctx->renderer = NULL;
    }
    if (ctx->window) {
        SDL_DestroyWindow(ctx->window);
        ctx->window = NULL;
    }
    if (ctx->initialized) {
        TTF_Quit();
        SDL_Quit();
        ctx->initialized = 0;
    }
}

void renderer_clear(renderer_context_t *ctx) {
    if (!ctx->renderer) return;

    // Dark racing background
    SDL_SetRenderDrawColor(ctx->renderer, 5, 5, 15, 255);
    SDL_RenderClear(ctx->renderer);
}

void renderer_present(renderer_context_t *ctx) {
    if (!ctx->renderer) return;
    SDL_RenderPresent(ctx->renderer);
}

void draw_circle_outline(renderer_context_t *ctx, int center_x, int center_y, int radius, SDL_Color color) {
    if (!ctx->renderer) return;

    SDL_SetRenderDrawColor(ctx->renderer, color.r, color.g, color.b, color.a);

    for (int angle = 0; angle < 360; angle++) {
        float rad = angle * M_PI / 180.0f;
        int x = center_x + (int)(radius * cos(rad));
        int y = center_y + (int)(radius * sin(rad));
        SDL_RenderPoint(ctx->renderer, x, y);
    }
}

void draw_thick_circle_outline(renderer_context_t *ctx, int center_x, int center_y, int radius, int thickness, SDL_Color color) {
    for (int i = 0; i < thickness; i++) {
        draw_circle_outline(ctx, center_x, center_y, radius - i, color);
    }
}

void draw_line_polar(renderer_context_t *ctx, int center_x, int center_y, int radius, float angle_deg, SDL_Color color) {
    if (!ctx->renderer) return;

    SDL_SetRenderDrawColor(ctx->renderer, color.r, color.g, color.b, color.a);

    float rad = angle_deg * M_PI / 180.0f;
    int end_x = center_x + (int)(radius * cos(rad));
    int end_y = center_y + (int)(radius * sin(rad));

    SDL_RenderLine(ctx->renderer, center_x, center_y, end_x, end_y);
}

void draw_arc_thick(renderer_context_t *ctx, int center_x, int center_y, int radius, float start_angle, float end_angle, int thickness, SDL_Color color) {
    if (!ctx->renderer) return;

    SDL_SetRenderDrawColor(ctx->renderer, color.r, color.g, color.b, color.a);

    for (int t = 0; t < thickness; t++) {
        int current_radius = radius - t;
        for (float angle = start_angle; angle <= end_angle; angle += 1.0f) {
            float rad = angle * M_PI / 180.0f;
            int x = center_x + (int)(current_radius * cos(rad));
            int y = center_y + (int)(current_radius * sin(rad));
            SDL_RenderPoint(ctx->renderer, x, y);
        }
    }
}

void render_text_with_font(renderer_context_t *ctx, TTF_Font *font_to_use, const char *text, int x, int y, SDL_Color color) {
    if (!font_to_use || !ctx->renderer || !text) return;

    SDL_Surface *text_surface = TTF_RenderText_Solid(font_to_use, text, 0, color);
    if (!text_surface) return;

    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(ctx->renderer, text_surface);
    if (!text_texture) {
        SDL_DestroySurface(text_surface);
        return;
    }

    SDL_FRect dst_rect = {x, y, text_surface->w, text_surface->h};
    SDL_RenderTexture(ctx->renderer, text_texture, NULL, &dst_rect);

    SDL_DestroyTexture(text_texture);
    SDL_DestroySurface(text_surface);
}

void render_text(renderer_context_t *ctx, const char *text, int x, int y, SDL_Color color) {
    render_text_with_font(ctx, ctx->font, text, x, y, color);
}

void render_text_large(renderer_context_t *ctx, const char *text, int x, int y, SDL_Color color) {
    render_text_with_font(ctx, ctx->large_font ? ctx->large_font : ctx->font, text, x, y, color);
}

void render_text_small(renderer_context_t *ctx, const char *text, int x, int y, SDL_Color color) {
    render_text_with_font(ctx, ctx->small_font ? ctx->small_font : ctx->font, text, x, y, color);
}
