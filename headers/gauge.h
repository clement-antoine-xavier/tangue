#pragma once

#include "window.h"

typedef struct gauge_s
{
	SDL_FRect position;
} gauge_t;

typedef struct gauge_value_s
{
	float maximum_value;
	float minimum_value;
	float red_line_value;
	float current_value;
} gauge_value_t;

int init_gauge(gauge_t **gauge, SDL_FRect *position);
int terminate_gauge(gauge_t *gauge);
void set_gauge_value(gauge_t *gauge, float value);
int draw_gauge(window_t *window, gauge_t *gauge, gauge_value_t *value);
