#include "gauge.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int init_gauge(gauge_t **gauge, SDL_FRect *position)
{
	*gauge = (gauge_t *)malloc(sizeof(gauge_t));
	if (*gauge == NULL)
	{
		fprintf(stderr, "[ERROR] [%s:%d] Memory allocation failed in init_gauge\n", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}

	(*gauge)->position = *position;

	return EXIT_SUCCESS;
}

int terminate_gauge(gauge_t *gauge)
{
	free(gauge);
	return EXIT_SUCCESS;
}

int draw_gauge(window_t *window, gauge_t *gauge, gauge_value_t *value)
{
	// Calculate center and radius (position is now the center)
	int center_x = gauge->position.x;
	int center_y = gauge->position.y;
	int radius = (gauge->position.w < gauge->position.h ? gauge->position.w : gauge->position.h) / 2 - 20;

	// Gauge spans from -135° to +135° (270° total)
	float start_angle = -135.0f * M_PI / 180.0f;
	float end_angle = 135.0f * M_PI / 180.0f;
	float angle_range = end_angle - start_angle;

	// Calculate current value angle
	float normalized_value = (value->current_value - value->minimum_value) / (value->maximum_value - value->minimum_value);
	float current_angle = start_angle + (normalized_value * angle_range);
	float redline_angle = start_angle + ((value->red_line_value - value->minimum_value) / (value->maximum_value - value->minimum_value)) * angle_range;

	// Draw background arc (dark gray)
	SDL_SetRenderDrawColor(window->sdl_renderer, 40, 40, 40, 255);
	for (float angle = start_angle; angle <= end_angle; angle += 0.01f)
	{
		for (int thickness = 0; thickness < 8; thickness++)
		{
			int px = center_x + (int)((radius - thickness) * cos(angle));
			int py = center_y + (int)((radius - thickness) * sin(angle));
			SDL_RenderPoint(window->sdl_renderer, px, py);
		}
	}

	// Draw value arc with gradient colors
	for (float angle = start_angle; angle <= current_angle && angle <= end_angle; angle += 0.01f)
	{
		// Color gradient from green to yellow to red
		float progress = (angle - start_angle) / angle_range;
		int r, g, b;

		if (progress < 0.6f) // Green to yellow
		{
			float local_progress = progress / 0.6f;
			r = (int)(local_progress * 255);
			g = 255;
			b = 0;
		}
		else if (progress < 0.8f) // Yellow to orange
		{
			float local_progress = (progress - 0.6f) / 0.2f;
			r = 255;
			g = (int)(255 - local_progress * 100);
			b = 0;
		}
		else // Orange to red
		{
			r = 255;
			g = (int)(155 * (1.0f - (progress - 0.8f) / 0.2f));
			b = 0;
		}

		SDL_SetRenderDrawColor(window->sdl_renderer, r, g, b, 255);

		for (int thickness = 0; thickness < 8; thickness++)
		{
			int px = center_x + (int)((radius - thickness) * cos(angle));
			int py = center_y + (int)((radius - thickness) * sin(angle));
			SDL_RenderPoint(window->sdl_renderer, px, py);
		}
	}

	// Draw major tick marks (every 20%)
	SDL_SetRenderDrawColor(window->sdl_renderer, 220, 220, 220, 255);
	for (int i = 0; i <= 5; i++)
	{
		float tick_angle = start_angle + (i * angle_range / 5);
		int outer_x = center_x + (int)((radius + 5) * cos(tick_angle));
		int outer_y = center_y + (int)((radius + 5) * sin(tick_angle));
		int inner_x = center_x + (int)((radius - 12) * cos(tick_angle));
		int inner_y = center_y + (int)((radius - 12) * sin(tick_angle));

		// Draw thicker major ticks
		for (int offset = -1; offset <= 1; offset++)
		{
			SDL_RenderLine(window->sdl_renderer, outer_x + offset, outer_y, inner_x + offset, inner_y);
			SDL_RenderLine(window->sdl_renderer, outer_x, outer_y + offset, inner_x, inner_y + offset);
		}
	}

	// Draw minor tick marks (every 10%)
	SDL_SetRenderDrawColor(window->sdl_renderer, 150, 150, 150, 255);
	for (int i = 0; i <= 10; i++)
	{
		if (i % 2 != 0) // Skip major ticks
		{
			float tick_angle = start_angle + (i * angle_range / 10);
			int outer_x = center_x + (int)((radius + 2) * cos(tick_angle));
			int outer_y = center_y + (int)((radius + 2) * sin(tick_angle));
			int inner_x = center_x + (int)((radius - 8) * cos(tick_angle));
			int inner_y = center_y + (int)((radius - 8) * sin(tick_angle));
			SDL_RenderLine(window->sdl_renderer, outer_x, outer_y, inner_x, inner_y);
		}
	}

	// Draw redline marker
	SDL_SetRenderDrawColor(window->sdl_renderer, 255, 50, 50, 255);
	int redline_outer_x = center_x + (int)((radius + 8) * cos(redline_angle));
	int redline_outer_y = center_y + (int)((radius + 8) * sin(redline_angle));
	int redline_inner_x = center_x + (int)((radius - 15) * cos(redline_angle));
	int redline_inner_y = center_y + (int)((radius - 15) * sin(redline_angle));

	for (int offset = -2; offset <= 2; offset++)
	{
		SDL_RenderLine(window->sdl_renderer, redline_outer_x + offset, redline_outer_y, redline_inner_x + offset, redline_inner_y);
		SDL_RenderLine(window->sdl_renderer, redline_outer_x, redline_outer_y + offset, redline_inner_x, redline_inner_y + offset);
	}

	// Draw modern needle with shadow effect
	int needle_length = radius - 25;
	int needle_x = center_x + (int)(needle_length * cos(current_angle));
	int needle_y = center_y + (int)(needle_length * sin(current_angle));

	// Shadow (offset by 2 pixels)
	SDL_SetRenderDrawColor(window->sdl_renderer, 0, 0, 0, 128);
	for (int width = -3; width <= 3; width++)
	{
		SDL_RenderLine(window->sdl_renderer,
					   center_x + 2 + width, center_y + 2,
					   needle_x + 2 + width, needle_y + 2);
		SDL_RenderLine(window->sdl_renderer,
					   center_x + 2, center_y + 2 + width,
					   needle_x + 2, needle_y + 2 + width);
	}

	// Main needle
	if (value->current_value >= value->red_line_value)
	{
		SDL_SetRenderDrawColor(window->sdl_renderer, 255, 80, 80, 255); // Bright red
	}
	else
	{
		SDL_SetRenderDrawColor(window->sdl_renderer, 255, 255, 255, 255); // White
	}

	for (int width = -3; width <= 3; width++)
	{
		SDL_RenderLine(window->sdl_renderer,
					   center_x + width, center_y,
					   needle_x + width, needle_y);
		SDL_RenderLine(window->sdl_renderer,
					   center_x, center_y + width,
					   needle_x, needle_y + width);
	}

	// Draw modern center hub with gradient effect
	int hub_radius = 8;
	for (int r = hub_radius; r >= 0; r--)
	{
		int intensity = 100 + (155 * r / hub_radius);
		SDL_SetRenderDrawColor(window->sdl_renderer, intensity, intensity, intensity, 255);

		for (int i = -r; i <= r; i++)
		{
			for (int j = -r; j <= r; j++)
			{
				if (i * i + j * j <= r * r)
				{
					SDL_RenderPoint(window->sdl_renderer, center_x + i, center_y + j);
				}
			}
		}
	}

	// Draw outer ring for depth
	SDL_SetRenderDrawColor(window->sdl_renderer, 100, 100, 100, 255);
	for (int thickness = 0; thickness < 3; thickness++)
	{
		for (int i = 0; i < 360; i += 2)
		{
			float angle = i * M_PI / 180.0f;
			int px = center_x + (int)((radius + 12 + thickness) * cos(angle));
			int py = center_y + (int)((radius + 12 + thickness) * sin(angle));
			SDL_RenderPoint(window->sdl_renderer, px, py);
		}
	}

	return EXIT_SUCCESS;
}
