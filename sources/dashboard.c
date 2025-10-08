#include "dashboard.h"

#include <stdlib.h>
#include <stdio.h>

int init_dashboard(dashboard_t **dashboard, char binding_address[INET_ADDRSTRLEN], unsigned short binding_port)
{
	*dashboard = (dashboard_t *)malloc(sizeof(dashboard_t));
	if (*dashboard == NULL)
	{
		fprintf(stderr, "[ERROR] [%s:%d] Memory allocation failed in init_dashboard\n", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}

	if (init_server(&(*dashboard)->server, binding_address, binding_port) != EXIT_SUCCESS)
	{
		fprintf(stderr, "[ERROR] [%s:%d] Failed to initialize server in init_dashboard\n", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}

	if (init_window(&(*dashboard)->window) != EXIT_SUCCESS)
	{
		fprintf(stderr, "[ERROR] [%s:%d] Failed to initialize window in init_dashboard\n", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}

	// Initialize center RPM gauge (bigger)
	if (init_gauge(&(*dashboard)->rpm_gauge,
		&(SDL_FRect){
			.x = (*dashboard)->window->screen_width / 2,  // Center X
			.y = (*dashboard)->window->screen_height / 2, // Center Y
			.w = 300,  // Width
			.h = 300   // Height
		}
	) != EXIT_SUCCESS)
	{
		fprintf(stderr, "[ERROR] [%s:%d] Failed to initialize RPM gauge in init_dashboard\n", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}

	// Initialize left gauge (smaller)
	if (init_gauge(&(*dashboard)->left_gauge,
		&(SDL_FRect){
			.x = (*dashboard)->window->screen_width / 4,     // Left quarter
			.y = (*dashboard)->window->screen_height / 2,    // Center Y
			.w = 200,  // Width
			.h = 200   // Height
		}
	) != EXIT_SUCCESS)
	{
		fprintf(stderr, "[ERROR] [%s:%d] Failed to initialize left gauge in init_dashboard\n", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}

	// Initialize right gauge (smaller)
	if (init_gauge(&(*dashboard)->right_gauge,
		&(SDL_FRect){
			.x = (*dashboard)->window->screen_width * 3 / 4, // Right quarter
			.y = (*dashboard)->window->screen_height / 2,     // Center Y
			.w = 200,  // Width
			.h = 200   // Height
		}
	) != EXIT_SUCCESS)
	{
		fprintf(stderr, "[ERROR] [%s:%d] Failed to initialize right gauge in init_dashboard\n", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}

	(*dashboard)->is_running = true;

	return EXIT_SUCCESS;
}

int terminate_dashboard(dashboard_t *dashboard)
{
	if (terminate_window(dashboard->window) != EXIT_SUCCESS)
	{
		fprintf(stderr, "[ERROR] [%s:%d] Failed to terminate window in terminate_dashboard\n", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}

	free(dashboard);
	return EXIT_SUCCESS;
}

bool dashboard_is_running(dashboard_t *dashboard)
{
	return dashboard->is_running;
}

int dashboard_routine(dashboard_t *dashboard)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_EVENT_QUIT)
			dashboard->is_running = false;
	}

	if (clear_window(dashboard->window) != EXIT_SUCCESS)
	{
		fprintf(stderr, "[ERROR] [%s:%d] Failed to clear window in dashboard_routine\n", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}

	static float rpm_value = 0.0f, left_value = 0.0f, right_value = 0.0f;

	// Update gauge values with different patterns
	rpm_value += 0.5f;
	if (rpm_value > 100.0f)
		rpm_value = 50.0f;

	left_value += 0.3f;
	if (left_value > 100.0f)
		left_value = 40.0f;

	right_value += 0.7f;
	if (right_value > 100.0f)
		right_value = 60.0f;

	// Draw center RPM gauge
	if (draw_gauge(dashboard->window, dashboard->rpm_gauge, &(gauge_value_t){
		.maximum_value = 100.0f,
		.minimum_value = 0.0f,
		.red_line_value = 75.0f,
		.current_value = rpm_value
	}) != EXIT_SUCCESS)
	{
		fprintf(stderr, "[ERROR] [%s:%d] Failed to draw RPM gauge in dashboard_routine\n", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}

	// Draw left gauge (e.g., temperature)
	if (draw_gauge(dashboard->window, dashboard->left_gauge, &(gauge_value_t){
		.maximum_value = 100.0f,
		.minimum_value = 0.0f,
		.red_line_value = 80.0f,
		.current_value = left_value
	}) != EXIT_SUCCESS)
	{
		fprintf(stderr, "[ERROR] [%s:%d] Failed to draw left gauge in dashboard_routine\n", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}

	// Draw right gauge (e.g., fuel level)
	if (draw_gauge(dashboard->window, dashboard->right_gauge, &(gauge_value_t){
		.maximum_value = 100.0f,
		.minimum_value = 0.0f,
		.red_line_value = 20.0f, // Low fuel warning
		.current_value = right_value
	}) != EXIT_SUCCESS)
	{
		fprintf(stderr, "[ERROR] [%s:%d] Failed to draw right gauge in dashboard_routine\n", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}

	if (present_window(dashboard->window) != EXIT_SUCCESS)
	{
		fprintf(stderr, "[ERROR] [%s:%d] Failed to present window in dashboard_routine\n", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
