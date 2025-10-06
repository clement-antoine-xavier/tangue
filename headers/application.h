#pragma once

#include "renderer.h"
#include "dashboard.h"
#include "network.h"
#include "telemetry.h"

// Application context structure
typedef struct {
    renderer_context_t renderer;
    dashboard_context_t dashboard;
    network_context_t network;
    telemetry_t telemetry;
    network_status_t network_status;
    int running;
    int verbose_mode;
} application_context_t;

// Function declarations
int application_init(application_context_t *ctx);
void application_cleanup(application_context_t *ctx);
void application_run(application_context_t *ctx);
void application_handle_events(application_context_t *ctx);
void application_update_telemetry(application_context_t *ctx);
void application_render(application_context_t *ctx);
int application_is_running(const application_context_t *ctx);
