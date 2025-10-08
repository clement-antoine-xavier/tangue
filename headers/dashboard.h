#pragma once

#include "server.h"
#include "window.h"
#include "telemetry.h"
#include "gauge.h"

typedef struct dashboard_s
{
	server_t *server;
	telemetry_t telemetry;
	window_t *window;
	bool is_running;
	gauge_t *rpm_gauge;
	gauge_t *left_gauge;
	gauge_t *right_gauge;
} dashboard_t;

int init_dashboard(dashboard_t **dashboard, char binding_address[INET_ADDRSTRLEN], unsigned short binding_port);
int terminate_dashboard(dashboard_t *dashboard);
bool dashboard_is_running(dashboard_t *dashboard);
int dashboard_routine(dashboard_t *dashboard);
