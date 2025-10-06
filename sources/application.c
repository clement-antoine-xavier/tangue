#include "application.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Default telemetry values for demonstration
static void init_default_telemetry(telemetry_t *telemetry) {
    memset(telemetry, 0, sizeof(telemetry_t));

    // Engine data
    telemetry->engine.max_rpm = 8000;
    telemetry->engine.current_rpm = 2000;
    telemetry->engine.idle_rpm = 800;

    // Car data
    telemetry->car.drivetrain_type = 2; // AWD
    telemetry->car.num_cylinders = 8;
    telemetry->car.performance_index = 850;

    // Vehicle state
    telemetry->speed = 60.0f; // m/s
    telemetry->power = 300000; // Watts
    telemetry->torque = 450.0f; // Nm
    telemetry->fuel = 0.75f; // 75%

    // Vehicle controls
    telemetry->vehicle_control.accel = 80;
    telemetry->vehicle_control.brake = 0;
    telemetry->vehicle_control.clutch = 0;
    telemetry->vehicle_control.handbrake = 0;
    telemetry->vehicle_control.gear = 4;
    telemetry->vehicle_control.steer = 128; // Center

    // Race data
    telemetry->is_race_on = 0;
    telemetry->race.lap_number = 1;
    telemetry->race.position = 5;
    telemetry->race.current_lap = 95.432f;
    telemetry->race.best_lap = 92.125f;
    telemetry->race.last_lap = 94.876f;
}

int application_init(application_context_t *ctx) {
    if (!ctx) return -1;

    memset(ctx, 0, sizeof(application_context_t));
    ctx->running = 0;
    ctx->verbose_mode = 0;

    // Initialize renderer
    if (renderer_init(&ctx->renderer, "Tangue Sports Car Dashboard") != 0) {
        fprintf(stderr, "Failed to initialize renderer\n");
        return -1;
    }

    // Initialize dashboard
    if (dashboard_init(&ctx->dashboard, &ctx->renderer) != 0) {
        fprintf(stderr, "Failed to initialize dashboard\n");
        renderer_cleanup(&ctx->renderer);
        return -1;
    }

    // Initialize network
    int port = 10000; // Default telemetry port
    if (network_init(&ctx->network, port) != 0) {
        fprintf(stderr, "Failed to initialize network on port %d\n", port);
        dashboard_cleanup(&ctx->dashboard);
        renderer_cleanup(&ctx->renderer);
        return -1;
    }

    // Initialize with default telemetry data
    init_default_telemetry(&ctx->telemetry);

    ctx->running = 1;
    return 0;
}

void application_cleanup(application_context_t *ctx) {
    if (!ctx) return;

    network_cleanup(&ctx->network);
    dashboard_cleanup(&ctx->dashboard);
    renderer_cleanup(&ctx->renderer);

    ctx->running = 0;
}

void application_handle_events(application_context_t *ctx) {
    if (!ctx) return;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                ctx->running = 0;
                break;

            case SDL_EVENT_KEY_DOWN:
                switch (event.key.key) {
                    case SDLK_ESCAPE:
                    case SDLK_Q:
                        ctx->running = 0;
                        break;

                    case SDLK_V:
                        ctx->verbose_mode = !ctx->verbose_mode;
                        break;

                    case SDLK_F11:
                        // Toggle fullscreen (if implemented in renderer)
                        break;

                    default:
                        break;
                }
                break;

            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                ctx->running = 0;
                break;

            default:
                break;
        }
    }
}

void application_update_telemetry(application_context_t *ctx) {
    if (!ctx) return;

    // Try to receive new telemetry data
    network_status_t status = network_receive_telemetry(&ctx->network, &ctx->telemetry);
    ctx->network_status = status;
    if (ctx->verbose_mode && status == NETWORK_SUCCESS) {
        printf("Received telemetry packet\n");
        printf("  Speed: %.1f km/h\n", ctx->telemetry.speed * 3.6f);
        printf("  RPM: %.0f\n", ctx->telemetry.engine.current_rpm);
        printf("  Gear: %d\n", ctx->telemetry.vehicle_control.gear);
        printf("  Fuel: %.1f%%\n", ctx->telemetry.fuel * 100.0f);
        printf("  Power: %.0f HP\n", ctx->telemetry.power * 0.00134102);
        printf("  Network status: %d\n", status);
        printf("---\n");
    }
}

void application_render(application_context_t *ctx) {
    if (!ctx) return;

    // Clear screen
    renderer_clear(&ctx->renderer);

    // Render dashboard
    dashboard_render(&ctx->dashboard, &ctx->telemetry);

    // Show network status in corner
    char status_info[64];
    snprintf(status_info, sizeof(status_info), "Network status: %d (Port %d)", ctx->network_status, ctx->network.port);
    render_text_small(&ctx->renderer, status_info, 10, 10, ctx->renderer.colors.yellow);
    if (ctx->verbose_mode) {
        render_text_small(&ctx->renderer, "VERBOSE MODE", WINDOW_WIDTH - 120, 10, ctx->renderer.colors.cyan);
    }
    renderer_present(&ctx->renderer);
}

void application_run(application_context_t *ctx) {
    if (!ctx || !ctx->running) {
        fprintf(stderr, "Application not properly initialized\n");
        return;
    }

    printf("Sports Car Dashboard started\n");
    printf("Listening for telemetry data on UDP port %d\n", ctx->network.port);
    printf("Press 'Q' or ESC to quit, 'V' to toggle verbose mode\n");
    printf("You can use the test_sender.py script to send sample data\n");
    printf("---\n");

    Uint64 last_frame_time = SDL_GetTicks();
    const Uint64 target_frame_time = 1000 / 60; // 60 FPS

    while (ctx->running) {
    Uint64 frame_start = SDL_GetTicks();

        // Handle events
        application_handle_events(ctx);

        // Update telemetry data
        application_update_telemetry(ctx);

        // Render frame
        application_render(ctx);

        // Frame rate limiting
    Uint64 frame_time = SDL_GetTicks() - frame_start;
        if (frame_time < target_frame_time) {
            SDL_Delay(target_frame_time - frame_time);
        }

    last_frame_time = SDL_GetTicks();
    }

    printf("Application shutting down...\n");
}

int application_is_running(const application_context_t *ctx) {
    return ctx ? ctx->running : 0;
}
