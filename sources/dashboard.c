#include "dashboard.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int dashboard_init(dashboard_context_t *ctx, renderer_context_t *renderer) {
    ctx->renderer = renderer;
    ctx->previous_lap_time = 0.0f;
    ctx->best_personal_lap = 0.0f;
    ctx->alert_flash_timer = 0;
    ctx->power_peak_hold = 0.0f;
    ctx->torque_peak_hold = 0.0f;
    return 0;
}

void dashboard_cleanup(dashboard_context_t *ctx) {
    ctx->renderer = NULL;
}

// Header section functions
void draw_race_status_led(dashboard_context_t *ctx, int is_race_on) {
    SDL_Color led_color = is_race_on ? ctx->renderer->colors.green : ctx->renderer->colors.red;

    // Draw LED circle
    draw_thick_circle_outline(ctx->renderer, RACE_STATUS_LED_X, RACE_STATUS_LED_Y,
                             RACE_STATUS_LED_SIZE/2, RACE_STATUS_LED_SIZE/2, led_color);

    // Add glow effect for race on
    if (is_race_on) {
        draw_thick_circle_outline(ctx->renderer, RACE_STATUS_LED_X, RACE_STATUS_LED_Y,
                                 RACE_STATUS_LED_SIZE/2 + 2, 2, led_color);
    }

    // Label
    const char* status_text = is_race_on ? "RACE ON" : "RACE OFF";
    render_text_small(ctx->renderer, status_text, RACE_STATUS_LED_X + 30, RACE_STATUS_LED_Y - 5, ctx->renderer->colors.white);
}

void draw_position_display(dashboard_context_t *ctx, int position) {
    char pos_str[16];
    const char* suffix = "th";

    // Determine ordinal suffix
    if (position % 10 == 1 && position % 100 != 11) suffix = "st";
    else if (position % 10 == 2 && position % 100 != 12) suffix = "nd";
    else if (position % 10 == 3 && position % 100 != 13) suffix = "rd";

    snprintf(pos_str, sizeof(pos_str), "%d%s", position, suffix);

    SDL_Color pos_color = {255, 215, 0, 255}; // Gold
    render_text_large(ctx->renderer, pos_str, POSITION_DISPLAY_X, POSITION_DISPLAY_Y, pos_color);
}

void draw_lap_counter(dashboard_context_t *ctx, int current_lap, int total_laps) {
    char lap_str[32];
    if (total_laps > 0) {
        snprintf(lap_str, sizeof(lap_str), "LAP %d/%d", current_lap, total_laps);
    } else {
        snprintf(lap_str, sizeof(lap_str), "LAP %d", current_lap);
    }

    render_text_large(ctx->renderer, lap_str, LAP_COUNTER_X, LAP_COUNTER_Y, ctx->renderer->colors.white);
}

void draw_distance_display(dashboard_context_t *ctx, float distance_km) {
    char dist_str[32];
    snprintf(dist_str, sizeof(dist_str), "%.1f km", distance_km);

    render_text_small(ctx->renderer, dist_str, DISTANCE_DISPLAY_X, DISTANCE_DISPLAY_Y, ctx->renderer->colors.cyan);
}

void draw_timestamp_display(dashboard_context_t *ctx, unsigned int timestamp_ms) {
    int hours = timestamp_ms / 3600000;
    int minutes = (timestamp_ms % 3600000) / 60000;
    int seconds = (timestamp_ms % 60000) / 1000;
    int milliseconds = timestamp_ms % 1000;

    char time_str[32];
    snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d.%03d", hours, minutes, seconds, milliseconds);

    render_text_small(ctx->renderer, time_str, TIMESTAMP_DISPLAY_X, TIMESTAMP_DISPLAY_Y, ctx->renderer->colors.white);
}

// Central cluster functions
void draw_speedometer_analog(dashboard_context_t *ctx, float speed_kmh) {
    SDL_Color speed_color = speed_kmh > 200 ? ctx->renderer->colors.red :
                           (speed_kmh > 100 ? ctx->renderer->colors.yellow : ctx->renderer->colors.cyan);

    draw_circular_gauge(ctx, SPEEDOMETER_X, SPEEDOMETER_Y, GAUGE_RADIUS, speed_kmh, 0, 300,
                        "SPEED", "km/h", speed_color, ctx->renderer->colors.white);

    // Digital readout
    char speed_str[16];
    snprintf(speed_str, sizeof(speed_str), "%.0f", speed_kmh);
    render_text_large(ctx->renderer, speed_str, SPEEDOMETER_X - 30, SPEEDOMETER_Y + 20, speed_color);
}

void draw_tachometer_analog(dashboard_context_t *ctx, const engine_t* engine) {
    float rpm_percent = (engine->current_rpm / engine->max_rpm) * 100.0f;
    SDL_Color rpm_color = rpm_percent > 90 ? ctx->renderer->colors.red :
                         (rpm_percent > 75 ? ctx->renderer->colors.yellow : ctx->renderer->colors.green);

    draw_circular_gauge(ctx, TACHOMETER_X, TACHOMETER_Y, GAUGE_RADIUS, engine->current_rpm,
                        0, engine->max_rpm, "RPM", "rpm", rpm_color, ctx->renderer->colors.white);

    // RPM digital readout
    char rpm_str[16];
    snprintf(rpm_str, sizeof(rpm_str), "%.0f", engine->current_rpm);
    render_text_large(ctx->renderer, rpm_str, TACHOMETER_X - 30, TACHOMETER_Y + 20, rpm_color);
}

void draw_gear_indicator_enhanced(dashboard_context_t *ctx, int gear, float rpm, float max_rpm) {
    SDL_Color bg_color = {20, 20, 20, 255};
    SDL_Color text_color;
    SDL_Color shift_light_color = {0, 0, 0, 0}; // Transparent by default

    // Color based on gear
    if (gear == 0) {
        text_color = ctx->renderer->colors.white;
    } else if (gear < 0) {
        text_color = ctx->renderer->colors.red; // Red for reverse
    } else {
        text_color = ctx->renderer->colors.green; // Green for forward gears
    }

    // Shift light logic
    float rpm_percent = (rpm / max_rpm) * 100.0f;
    if (rpm_percent > 85) {
        shift_light_color = ctx->renderer->colors.red;
        // Flash at redline
        if (rpm_percent > 95 && (ctx->alert_flash_timer % 20) < 10) {
            shift_light_color = ctx->renderer->colors.white;
        }
    }

    char gear_str[8];
    if (gear == 0) {
        strcpy(gear_str, "N");
    } else if (gear < 0) {
        strcpy(gear_str, "R");
    } else {
        snprintf(gear_str, sizeof(gear_str), "%d", gear);
    }

    draw_digital_display(ctx, GEAR_DISPLAY_X, GEAR_DISPLAY_Y, 100, 100, gear_str, bg_color, text_color, ctx->renderer->large_font);

    // Draw shift light above gear indicator
    if (shift_light_color.a > 0) {
        SDL_FRect shift_light = {GEAR_DISPLAY_X + 10, GEAR_DISPLAY_Y - 15, 80, 10};
        SDL_SetRenderDrawColor(ctx->renderer->renderer, shift_light_color.r, shift_light_color.g, shift_light_color.b, 255);
        SDL_RenderFillRect(ctx->renderer->renderer, &shift_light);
    }
}

// Lap time functions with delta calculations
void calculate_lap_delta(float current_time, float reference_time, char* delta_str, SDL_Color* color) {
    if (reference_time <= 0.0f || current_time <= 0.0f) {
        strcpy(delta_str, "");
        *color = (SDL_Color){255, 255, 255, 255};
        return;
    }

    float delta = current_time - reference_time;
    if (delta < 0) {
        snprintf(delta_str, 32, "%.3fs", delta);
        *color = (SDL_Color){0, 255, 0, 255}; // Green for improvement
    } else {
        snprintf(delta_str, 32, "+%.3fs", delta);
        *color = (SDL_Color){255, 0, 0, 255}; // Red for loss
    }
}

// Helper function to format lap times
void format_lap_time(float time_seconds, char* buffer) {
    if (time_seconds <= 0) {
        strcpy(buffer, "--:--.---");
        return;
    }
    int minutes = (int)(time_seconds / 60);
    float seconds = time_seconds - (minutes * 60);
    snprintf(buffer, 32, "%d:%06.3f", minutes, seconds);
}

void draw_lap_times_with_delta(dashboard_context_t *ctx, const race_t* race) {
    SDL_Color bg_color = {30, 30, 30, 255};
    SDL_Color time_color = ctx->renderer->colors.yellow;

    // Best lap
    char best_time_str[32];
    format_lap_time(race->best_lap, best_time_str);
    draw_digital_display(ctx, LAP_TIME_PANEL_X, LAP_TIME_PANEL_Y, LAP_TIME_WIDTH, LAP_TIME_HEIGHT,
                        "", bg_color, time_color, ctx->renderer->font);
    render_text_small(ctx->renderer, "Best:", LAP_TIME_PANEL_X + 5, LAP_TIME_PANEL_Y + 2, ctx->renderer->colors.white);
    render_text_small(ctx->renderer, best_time_str, LAP_TIME_PANEL_X + 50, LAP_TIME_PANEL_Y + 2, time_color);

    // Last lap with delta to best
    char last_time_str[32];
    format_lap_time(race->last_lap, last_time_str);
    char delta_str[32];
    SDL_Color delta_color;
    calculate_lap_delta(race->last_lap, race->best_lap, delta_str, &delta_color);

    draw_digital_display(ctx, LAP_TIME_PANEL_X, LAP_TIME_PANEL_Y + 30, LAP_TIME_WIDTH, LAP_TIME_HEIGHT,
                        "", bg_color, time_color, ctx->renderer->font);
    render_text_small(ctx->renderer, "Last:", LAP_TIME_PANEL_X + 5, LAP_TIME_PANEL_Y + 32, ctx->renderer->colors.white);
    render_text_small(ctx->renderer, last_time_str, LAP_TIME_PANEL_X + 50, LAP_TIME_PANEL_Y + 32, time_color);
    if (strlen(delta_str) > 0) {
        render_text_small(ctx->renderer, delta_str, LAP_TIME_PANEL_X + 130, LAP_TIME_PANEL_Y + 32, delta_color);
    }

    // Current lap with delta to best
    char current_time_str[32];
    format_lap_time(race->current_lap, current_time_str);
    calculate_lap_delta(race->current_lap, race->best_lap, delta_str, &delta_color);

    draw_digital_display(ctx, LAP_TIME_PANEL_X, LAP_TIME_PANEL_Y + 60, LAP_TIME_WIDTH, LAP_TIME_HEIGHT,
                        "", bg_color, ctx->renderer->colors.cyan, ctx->renderer->font);
    render_text_small(ctx->renderer, "Current:", LAP_TIME_PANEL_X + 5, LAP_TIME_PANEL_Y + 62, ctx->renderer->colors.white);
    render_text_small(ctx->renderer, current_time_str, LAP_TIME_PANEL_X + 50, LAP_TIME_PANEL_Y + 62, ctx->renderer->colors.cyan);
    if (strlen(delta_str) > 0) {
        render_text_small(ctx->renderer, delta_str, LAP_TIME_PANEL_X + 130, LAP_TIME_PANEL_Y + 62, delta_color);
    }
}

// Acceleration and velocity displays
void draw_acceleration_bars_3axis(dashboard_context_t *ctx, const vector3_t* acceleration) {
    SDL_Color bg_color = {20, 20, 20, 255};

    // X-axis (longitudinal) - horizontal bar
    float accel_x_g = acceleration->x / 9.81f; // Convert to G-forces
    SDL_Color x_color = accel_x_g > 0 ? ctx->renderer->colors.green : ctx->renderer->colors.red;
    draw_progress_bar(ctx, ACCELERATION_BARS_X, ACCELERATION_BARS_Y, 120, 18,
                     fabs(accel_x_g), 3.0f, "X (Lon)", x_color, bg_color);
    char x_str[16];
    snprintf(x_str, sizeof(x_str), "%.2fG", accel_x_g);
    render_text_small(ctx->renderer, x_str, ACCELERATION_BARS_X + 130, ACCELERATION_BARS_Y + 2, ctx->renderer->colors.white);    // Y-axis (lateral) - horizontal bar
    float accel_y_g = acceleration->y / 9.81f;
    SDL_Color y_color = accel_y_g > 0 ? ctx->renderer->colors.cyan : ctx->renderer->colors.yellow;
    draw_progress_bar(ctx, ACCELERATION_BARS_X, ACCELERATION_BARS_Y + 25, 120, 18,
                     fabs(accel_y_g), 3.0f, "Y (Lat)", y_color, bg_color);
    char y_str[16];
    snprintf(y_str, sizeof(y_str), "%.2fG", accel_y_g);
    render_text_small(ctx->renderer, y_str, ACCELERATION_BARS_X + 130, ACCELERATION_BARS_Y + 27, ctx->renderer->colors.white);    // Z-axis (vertical) - horizontal bar
    float accel_z_g = acceleration->z / 9.81f;
    SDL_Color z_color = ctx->renderer->colors.white;
    draw_progress_bar(ctx, ACCELERATION_BARS_X, ACCELERATION_BARS_Y + 40, 120, 15,
                     fabs(accel_z_g), 2.0f, "Z (Ver)", z_color, bg_color);
    char z_str[16];
    snprintf(z_str, sizeof(z_str), "%.2fG", accel_z_g);
    render_text_small(ctx->renderer, z_str, ACCELERATION_BARS_X + 125, ACCELERATION_BARS_Y + 40, ctx->renderer->colors.white);
}

void draw_velocity_vector_3d(dashboard_context_t *ctx, const vector3_t* velocity) {
    int center_x = VELOCITY_ARROW_X + 60;
    int center_y = VELOCITY_ARROW_Y + 60;

    // Calculate magnitude and normalize
    float magnitude = sqrt(velocity->x * velocity->x + velocity->y * velocity->y + velocity->z * velocity->z);
    if (magnitude < 0.1f) magnitude = 0.1f; // Avoid division by zero

    // Scale for display (50 pixels max arrow length)
    float scale = 50.0f / magnitude;
    if (scale > 1.0f) scale = 1.0f;

    int arrow_x = (int)(velocity->x * scale);
    int arrow_y = -(int)(velocity->y * scale); // Invert Y for screen coordinates

    // Draw background circle
    draw_thick_circle_outline(ctx->renderer, center_x, center_y, 55, 2, ctx->renderer->colors.dark_gray);

    // Draw velocity arrow
    SDL_Color arrow_color = ctx->renderer->colors.cyan;
    SDL_SetRenderDrawColor(ctx->renderer->renderer, arrow_color.r, arrow_color.g, arrow_color.b, 255);

    // Main arrow line
    SDL_RenderLine(ctx->renderer->renderer, center_x, center_y, center_x + arrow_x, center_y + arrow_y);

    // Arrow head
    if (magnitude > 1.0f) {
        float angle = atan2(arrow_y, arrow_x);
        int head_size = 8;
        int head1_x = center_x + arrow_x - (int)(head_size * cos(angle - 0.5));
        int head1_y = center_y + arrow_y - (int)(head_size * sin(angle - 0.5));
        int head2_x = center_x + arrow_x - (int)(head_size * cos(angle + 0.5));
        int head2_y = center_y + arrow_y - (int)(head_size * sin(angle + 0.5));

        SDL_RenderLine(ctx->renderer->renderer, center_x + arrow_x, center_y + arrow_y, head1_x, head1_y);
        SDL_RenderLine(ctx->renderer->renderer, center_x + arrow_x, center_y + arrow_y, head2_x, head2_y);
    }

    // Draw magnitude text
    char mag_str[16];
    snprintf(mag_str, sizeof(mag_str), "%.1f m/s", magnitude);
    render_text_small(ctx->renderer, "VELOCITY", VELOCITY_ARROW_X + 15, VELOCITY_ARROW_Y - 20, ctx->renderer->colors.white);
    render_text_small(ctx->renderer, mag_str, VELOCITY_ARROW_X + 15, VELOCITY_ARROW_Y + 130, ctx->renderer->colors.cyan);
}

void draw_angular_velocity_dials(dashboard_context_t *ctx, const vector3_t* angular_velocity) {
    // X-axis (roll) dial
    draw_circular_gauge(ctx, ANGULAR_VELOCITY_X + 40, ANGULAR_VELOCITY_Y + 40, MINI_GAUGE_RADIUS,
                       angular_velocity->x, -3.14f, 3.14f, "ROLL", "rad/s",
                       ctx->renderer->colors.green, ctx->renderer->colors.white);

    // Y-axis (pitch) dial
    draw_circular_gauge(ctx, ANGULAR_VELOCITY_X + 120, ANGULAR_VELOCITY_Y + 40, MINI_GAUGE_RADIUS,
                       angular_velocity->y, -3.14f, 3.14f, "PITCH", "rad/s",
                       ctx->renderer->colors.yellow, ctx->renderer->colors.white);

    // Z-axis (yaw) dial
    draw_circular_gauge(ctx, ANGULAR_VELOCITY_X + 200, ANGULAR_VELOCITY_Y + 40, MINI_GAUGE_RADIUS,
                       angular_velocity->z, -3.14f, 3.14f, "YAW", "rad/s",
                       ctx->renderer->colors.red, ctx->renderer->colors.white);
}

// Wheel telemetry section
void draw_suspension_travel_bars(dashboard_context_t *ctx, const wheel_values_t* suspension) {
    SDL_Color bg_color = {30, 30, 30, 255};
    SDL_Color bar_color = ctx->renderer->colors.cyan;

    // FL
    draw_vertical_bar(ctx, WHEEL_FL_X, WHEEL_SECTION_Y, WHEEL_BAR_WIDTH, WHEEL_BAR_HEIGHT,
                     suspension->front_left, 1.0f, "FL", bar_color, bg_color);

    // FR
    draw_vertical_bar(ctx, WHEEL_FR_X, WHEEL_SECTION_Y, WHEEL_BAR_WIDTH, WHEEL_BAR_HEIGHT,
                     suspension->front_right, 1.0f, "FR", bar_color, bg_color);

    // RL
    draw_vertical_bar(ctx, WHEEL_RL_X, WHEEL_SECTION_Y, WHEEL_BAR_WIDTH, WHEEL_BAR_HEIGHT,
                     suspension->rear_left, 1.0f, "RL", bar_color, bg_color);

    // RR
    draw_vertical_bar(ctx, WHEEL_RR_X, WHEEL_SECTION_Y, WHEEL_BAR_WIDTH, WHEEL_BAR_HEIGHT,
                     suspension->rear_right, 1.0f, "RR", bar_color, bg_color);

    // Label
    render_text_small(ctx->renderer, "SUSPENSION", WHEEL_FL_X, WHEEL_SECTION_Y - 20, ctx->renderer->colors.white);
}

void draw_tire_slip_gauges(dashboard_context_t *ctx, const wheel_values_t* slip_ratio, const wheel_values_t* slip_angle) {
    // Slip ratio gauges (0-1 range)
    SDL_Color optimal_color = ctx->renderer->colors.green;
    SDL_Color warning_color = ctx->renderer->colors.yellow;
    SDL_Color critical_color = ctx->renderer->colors.red;

    // FL slip ratio
    SDL_Color fl_slip_color = (slip_ratio->front_left < 0.1f) ? optimal_color :
                             (slip_ratio->front_left < 0.3f) ? warning_color : critical_color;
    draw_radial_progress_gauge(ctx, WHEEL_FL_X + 30, WHEEL_SECTION_Y + 40, WHEEL_GAUGE_RADIUS,
                              slip_ratio->front_left, "SR", fl_slip_color);

    // FR slip ratio
    SDL_Color fr_slip_color = (slip_ratio->front_right < 0.1f) ? optimal_color :
                             (slip_ratio->front_right < 0.3f) ? warning_color : critical_color;
    draw_radial_progress_gauge(ctx, WHEEL_FR_X + 30, WHEEL_SECTION_Y + 40, WHEEL_GAUGE_RADIUS,
                              slip_ratio->front_right, "SR", fr_slip_color);

    // RL slip ratio
    SDL_Color rl_slip_color = (slip_ratio->rear_left < 0.1f) ? optimal_color :
                             (slip_ratio->rear_left < 0.3f) ? warning_color : critical_color;
    draw_radial_progress_gauge(ctx, WHEEL_RL_X + 30, WHEEL_SECTION_Y + 40, WHEEL_GAUGE_RADIUS,
                              slip_ratio->rear_left, "SR", rl_slip_color);

    // RR slip ratio
    SDL_Color rr_slip_color = (slip_ratio->rear_right < 0.1f) ? optimal_color :
                             (slip_ratio->rear_right < 0.3f) ? warning_color : critical_color;
    draw_radial_progress_gauge(ctx, WHEEL_RR_X + 30, WHEEL_SECTION_Y + 40, WHEEL_GAUGE_RADIUS,
                              slip_ratio->rear_right, "SR", rr_slip_color);

    // Slip angle gauges (-30° to +30° range) - positioned below slip ratio
    float angle_range = 30.0f; // degrees

    // FL slip angle
    float fl_angle_norm = (slip_angle->front_left + angle_range) / (2 * angle_range);
    draw_radial_progress_gauge(ctx, WHEEL_FL_X + 60, WHEEL_SECTION_Y + 40, WHEEL_GAUGE_RADIUS,
                              fl_angle_norm, "SA", ctx->renderer->colors.yellow);

    // FR slip angle
    float fr_angle_norm = (slip_angle->front_right + angle_range) / (2 * angle_range);
    draw_radial_progress_gauge(ctx, WHEEL_FR_X + 60, WHEEL_SECTION_Y + 40, WHEEL_GAUGE_RADIUS,
                              fr_angle_norm, "SA", ctx->renderer->colors.yellow);

    // RL slip angle
    float rl_angle_norm = (slip_angle->rear_left + angle_range) / (2 * angle_range);
    draw_radial_progress_gauge(ctx, WHEEL_RL_X + 60, WHEEL_SECTION_Y + 40, WHEEL_GAUGE_RADIUS,
                              rl_angle_norm, "SA", ctx->renderer->colors.yellow);

    // RR slip angle
    float rr_angle_norm = (slip_angle->rear_right + angle_range) / (2 * angle_range);
    draw_radial_progress_gauge(ctx, WHEEL_RR_X + 60, WHEEL_SECTION_Y + 40, WHEEL_GAUGE_RADIUS,
                              rr_angle_norm, "SA", ctx->renderer->colors.yellow);
}

// Helper function for tire temperature colors
SDL_Color get_tire_temp_color(dashboard_context_t *ctx, float temp) {
    if (temp < 180) return ctx->renderer->colors.cyan;
    if (temp < 220) return ctx->renderer->colors.green;
    if (temp < 280) return ctx->renderer->colors.yellow;
    return ctx->renderer->colors.red;
}

void draw_tire_temperature_bars(dashboard_context_t *ctx, const wheel_values_t* tire_temps) {
    // FL tire temp
    SDL_Color fl_color = get_tire_temp_color(ctx, tire_temps->front_left);
    draw_vertical_bar(ctx, WHEEL_FL_X + 90, WHEEL_SECTION_Y, WHEEL_BAR_WIDTH, WHEEL_BAR_HEIGHT,
                     tire_temps->front_left, 300.0f, "T", fl_color, (SDL_Color){20, 20, 20, 255});

    // FR tire temp
    SDL_Color fr_color = get_tire_temp_color(ctx, tire_temps->front_right);
    draw_vertical_bar(ctx, WHEEL_FR_X + 90, WHEEL_SECTION_Y, WHEEL_BAR_WIDTH, WHEEL_BAR_HEIGHT,
                     tire_temps->front_right, 300.0f, "T", fr_color, (SDL_Color){20, 20, 20, 255});

    // RL tire temp
    SDL_Color rl_color = get_tire_temp_color(ctx, tire_temps->rear_left);
    draw_vertical_bar(ctx, WHEEL_RL_X + 90, WHEEL_SECTION_Y, WHEEL_BAR_WIDTH, WHEEL_BAR_HEIGHT,
                     tire_temps->rear_left, 300.0f, "T", rl_color, (SDL_Color){20, 20, 20, 255});

    // RR tire temp
    SDL_Color rr_color = get_tire_temp_color(ctx, tire_temps->rear_right);
    draw_vertical_bar(ctx, WHEEL_RR_X + 90, WHEEL_SECTION_Y, WHEEL_BAR_WIDTH, WHEEL_BAR_HEIGHT,
                     tire_temps->rear_right, 300.0f, "T", rr_color, (SDL_Color){20, 20, 20, 255});

    // Label
    render_text_small(ctx->renderer, "TIRE TEMPS", WHEEL_FL_X + 90, WHEEL_SECTION_Y - 20, ctx->renderer->colors.white);
}

void draw_wheel_surface_indicators(dashboard_context_t *ctx, const wheel_values_t* rumble,
                                  const wheel_values_t* puddle, const wheel_values_t* rumble_strip) {
    int icon_size = 12;

    // Rumble strip indicators (vibration waves icon)
    if (rumble_strip->front_left > 0) {
        SDL_Color rumble_color = ctx->renderer->colors.yellow;
        SDL_SetRenderDrawColor(ctx->renderer->renderer, rumble_color.r, rumble_color.g, rumble_color.b, 255);
        // Draw wavy lines
        for (int i = 0; i < 3; i++) {
            SDL_RenderLine(ctx->renderer->renderer, WHEEL_FL_X + 110, WHEEL_SECTION_Y + 20 + i*3,
                          WHEEL_FL_X + 110 + icon_size, WHEEL_SECTION_Y + 20 + i*3);
        }
    }

    if (rumble_strip->front_right > 0) {
        SDL_Color rumble_color = ctx->renderer->colors.yellow;
        SDL_SetRenderDrawColor(ctx->renderer->renderer, rumble_color.r, rumble_color.g, rumble_color.b, 255);
        for (int i = 0; i < 3; i++) {
            SDL_RenderLine(ctx->renderer->renderer, WHEEL_FR_X + 110, WHEEL_SECTION_Y + 20 + i*3,
                          WHEEL_FR_X + 110 + icon_size, WHEEL_SECTION_Y + 20 + i*3);
        }
    }

    if (rumble_strip->rear_left > 0) {
        SDL_Color rumble_color = ctx->renderer->colors.yellow;
        SDL_SetRenderDrawColor(ctx->renderer->renderer, rumble_color.r, rumble_color.g, rumble_color.b, 255);
        for (int i = 0; i < 3; i++) {
            SDL_RenderLine(ctx->renderer->renderer, WHEEL_RL_X + 110, WHEEL_SECTION_Y + 20 + i*3,
                          WHEEL_RL_X + 110 + icon_size, WHEEL_SECTION_Y + 20 + i*3);
        }
    }

    if (rumble_strip->rear_right > 0) {
        SDL_Color rumble_color = ctx->renderer->colors.yellow;
        SDL_SetRenderDrawColor(ctx->renderer->renderer, rumble_color.r, rumble_color.g, rumble_color.b, 255);
        for (int i = 0; i < 3; i++) {
            SDL_RenderLine(ctx->renderer->renderer, WHEEL_RR_X + 110, WHEEL_SECTION_Y + 20 + i*3,
                          WHEEL_RR_X + 110 + icon_size, WHEEL_SECTION_Y + 20 + i*3);
        }
    }

    // Puddle depth indicators (water droplet icon)
    SDL_Color water_color = ctx->renderer->colors.cyan;

    if (puddle->front_left > 0) {
        draw_vertical_bar(ctx, WHEEL_FL_X + 125, WHEEL_SECTION_Y + 40, 8, 30,
                         puddle->front_left, 1.0f, "", water_color, (SDL_Color){20, 20, 60, 255});
    }

    if (puddle->front_right > 0) {
        draw_vertical_bar(ctx, WHEEL_FR_X + 125, WHEEL_SECTION_Y + 40, 8, 30,
                         puddle->front_right, 1.0f, "", water_color, (SDL_Color){20, 20, 60, 255});
    }

    if (puddle->rear_left > 0) {
        draw_vertical_bar(ctx, WHEEL_RL_X + 125, WHEEL_SECTION_Y + 40, 8, 30,
                         puddle->rear_left, 1.0f, "", water_color, (SDL_Color){20, 20, 60, 255});
    }

    if (puddle->rear_right > 0) {
        draw_vertical_bar(ctx, WHEEL_RR_X + 125, WHEEL_SECTION_Y + 40, 8, 30,
                         puddle->rear_right, 1.0f, "", water_color, (SDL_Color){20, 20, 60, 255});
    }
}

void draw_wheel_telemetry_section(dashboard_context_t *ctx, const telemetry_t* telemetry) {
    draw_suspension_travel_bars(ctx, &telemetry->normalized_suspension_travel);
    draw_tire_slip_gauges(ctx, &telemetry->tire_slip_ratio, &telemetry->tire_slip_angle);
    draw_tire_temperature_bars(ctx, &telemetry->tire_temp);
    draw_wheel_surface_indicators(ctx, &telemetry->surface_rumble, &telemetry->wheel_in_puddle_depth,
                                 &telemetry->wheel_on_rumble_strip);
}

// Power and torque with peak hold
void draw_power_gauge_with_peak(dashboard_context_t *ctx, float power_watts) {
    float power_kw = power_watts / 1000.0f;

    // Update peak hold
    if (power_kw > ctx->power_peak_hold) {
        ctx->power_peak_hold = power_kw;
    }

    // Gradually decay peak hold
    ctx->power_peak_hold *= 0.999f;

    draw_circular_gauge(ctx, POWER_GAUGE_X, POWER_GAUGE_Y, SMALL_GAUGE_RADIUS, power_kw, 0, 600,
                        "POWER", "kW", ctx->renderer->colors.red, ctx->renderer->colors.white);

    // Draw peak hold marker
    if (ctx->power_peak_hold > 0) {
        float peak_angle = 210 + (120 * (ctx->power_peak_hold / 600.0f));
        float peak_rad = peak_angle * M_PI / 180.0f;
        int peak_x = POWER_GAUGE_X + (int)((SMALL_GAUGE_RADIUS - 20) * cos(peak_rad));
        int peak_y = POWER_GAUGE_Y + (int)((SMALL_GAUGE_RADIUS - 20) * sin(peak_rad));

        SDL_SetRenderDrawColor(ctx->renderer->renderer, 255, 255, 0, 255); // Yellow
        draw_thick_circle_outline(ctx->renderer, peak_x, peak_y, 3, 3, ctx->renderer->colors.yellow);
    }
}

void draw_torque_gauge_with_peak(dashboard_context_t *ctx, float torque_nm) {
    // Update peak hold
    if (torque_nm > ctx->torque_peak_hold) {
        ctx->torque_peak_hold = torque_nm;
    }

    // Gradually decay peak hold
    ctx->torque_peak_hold *= 0.999f;

    draw_circular_gauge(ctx, TORQUE_GAUGE_X, TORQUE_GAUGE_Y, SMALL_GAUGE_RADIUS, torque_nm, 0, 1000,
                        "TORQUE", "Nm", ctx->renderer->colors.orange, ctx->renderer->colors.white);

    // Draw peak hold marker
    if (ctx->torque_peak_hold > 0) {
        float peak_angle = 210 + (120 * (ctx->torque_peak_hold / 1000.0f));
        float peak_rad = peak_angle * M_PI / 180.0f;
        int peak_x = TORQUE_GAUGE_X + (int)((SMALL_GAUGE_RADIUS - 20) * cos(peak_rad));
        int peak_y = TORQUE_GAUGE_Y + (int)((SMALL_GAUGE_RADIUS - 20) * sin(peak_rad));

        SDL_SetRenderDrawColor(ctx->renderer->renderer, 255, 255, 0, 255); // Yellow
        draw_thick_circle_outline(ctx->renderer, peak_x, peak_y, 3, 3, ctx->renderer->colors.yellow);
    }
}

void draw_boost_gauge_colored(dashboard_context_t *ctx, float boost_psi) {
    SDL_Color boost_color;

    // Color bands for boost levels
    if (boost_psi > 25) {
        boost_color = ctx->renderer->colors.red; // Critical
    } else if (boost_psi > 15) {
        boost_color = ctx->renderer->colors.yellow; // Caution
    } else {
        boost_color = ctx->renderer->colors.green; // Safe
    }

    draw_circular_gauge(ctx, BOOST_GAUGE_X, BOOST_GAUGE_Y, SMALL_GAUGE_RADIUS, boost_psi, 0, 30,
                        "BOOST", "PSI", boost_color, ctx->renderer->colors.white);
}

// Enhanced fuel display
void draw_fuel_horizontal_bar(dashboard_context_t *ctx, float fuel_percent) {
    SDL_Color fuel_color;
    SDL_Color bg_color = {40, 40, 40, 255};

    // Color based on fuel level
    if (fuel_percent > 50) {
        fuel_color = ctx->renderer->colors.green;
    } else if (fuel_percent > 10) {
        fuel_color = ctx->renderer->colors.yellow;
    } else if (fuel_percent > 5) {
        fuel_color = ctx->renderer->colors.orange;
    } else {
        fuel_color = ctx->renderer->colors.red;
    }

    draw_progress_bar(ctx, FUEL_BAR_X, FUEL_BAR_Y, 200, 25, fuel_percent, 100, "FUEL", fuel_color, bg_color);

    // Percentage text
    char fuel_str[16];
    snprintf(fuel_str, sizeof(fuel_str), "%.1f%%", fuel_percent);
    render_text_small(ctx->renderer, fuel_str, FUEL_BAR_X + 210, FUEL_BAR_Y + 5, ctx->renderer->colors.white);

    // Flash warning for low fuel
    if (fuel_percent < 10 && (ctx->alert_flash_timer % 40) < 20) {
        SDL_Color warning_bg = {100, 0, 0, 255};
        draw_digital_display(ctx, FUEL_BAR_X, FUEL_BAR_Y - 35, 120, 28, "LOW FUEL",
                            warning_bg, ctx->renderer->colors.white, ctx->renderer->small_font);
    }
}

// Driving aids displays
void draw_driving_line_gauge(dashboard_context_t *ctx, unsigned char driving_line_accuracy) {
    float accuracy_percent = (driving_line_accuracy / 255.0f) * 100.0f;
    SDL_Color line_color = accuracy_percent > 80 ? ctx->renderer->colors.green :
                          (accuracy_percent > 60 ? ctx->renderer->colors.yellow : ctx->renderer->colors.red);

    draw_radial_progress_gauge(ctx, WINDOW_WIDTH/2, WINDOW_HEIGHT - 70, 40,
                              accuracy_percent / 100.0f, "LINE", line_color);

    char accuracy_str[16];
    snprintf(accuracy_str, sizeof(accuracy_str), "%.0f%%", accuracy_percent);
    render_text_small(ctx->renderer, accuracy_str, WINDOW_WIDTH/2 - 15, WINDOW_HEIGHT - 30, ctx->renderer->colors.white);
}

void draw_ai_brake_difference_bar(dashboard_context_t *ctx, unsigned char ai_brake_diff) {
    float diff_percent = (ai_brake_diff / 255.0f) * 100.0f;
    SDL_Color diff_color = diff_percent < 20 ? ctx->renderer->colors.green :
                          (diff_percent < 50 ? ctx->renderer->colors.yellow : ctx->renderer->colors.red);

    draw_progress_bar(ctx, WINDOW_WIDTH - 220, WINDOW_HEIGHT - 70, 160, 18, diff_percent, 100,
                     "AI BRAKE DIFF", diff_color, (SDL_Color){30, 30, 30, 255});
}

// Alert system
void draw_object_hit_alerts(dashboard_context_t *ctx, const object_hit_t* object_hits) {
    // Check if any objects were hit
    int has_hits = 0;
    for (int i = 0; i < 6; i++) {
        if (object_hits->obj[i] != 0) {
            has_hits = 1;
            break;
        }
    }

    if (has_hits && (ctx->alert_flash_timer % 30) < 15) {
        SDL_Color alert_bg = {150, 0, 0, 255};
        SDL_Color alert_text = {255, 255, 255, 255};

        draw_digital_display(ctx, ALERT_BOX_X, ALERT_BOX_Y, ALERT_BOX_WIDTH, ALERT_BOX_HEIGHT,
                            "COLLISION DETECTED!", alert_bg, alert_text, ctx->renderer->font);

        // Show hit object icons
        char hit_objects[64] = "";
        for (int i = 0; i < 6 && i < 3; i++) { // Show max 3 objects
            if (object_hits->obj[i] != 0) {
                char obj_str[8];
                snprintf(obj_str, sizeof(obj_str), "[%c] ", object_hits->obj[i]);
                strcat(hit_objects, obj_str);
            }
        }

        if (strlen(hit_objects) > 0) {
            render_text_small(ctx->renderer, hit_objects, ALERT_BOX_X + 10, ALERT_BOX_Y + ALERT_BOX_HEIGHT + 5,
                             ctx->renderer->colors.white);
        }
    }
}

// Enhanced control inputs
void draw_steering_wheel_graphic(dashboard_context_t *ctx, unsigned char steer_value) {
    int center_x = STEERING_DISPLAY_X + 25;
    int center_y = CONTROLS_Y + 25;
    int wheel_radius = 20;

    // Draw steering wheel circle
    draw_thick_circle_outline(ctx->renderer, center_x, center_y, wheel_radius, 2, ctx->renderer->colors.white);

    // Calculate steering angle (-45° to +45° based on steer_value 0-255)
    float steer_angle = ((steer_value - 128) / 127.0f) * 45.0f; // degrees
    float steer_rad = steer_angle * M_PI / 180.0f;

    // Draw steering indicator line
    int indicator_x = center_x + (int)(wheel_radius * 0.8f * sin(steer_rad));
    int indicator_y = center_y - (int)(wheel_radius * 0.8f * cos(steer_rad));

    SDL_SetRenderDrawColor(ctx->renderer->renderer, 255, 255, 0, 255); // Yellow
    SDL_RenderLine(ctx->renderer->renderer, center_x, center_y, indicator_x, indicator_y);

    // Draw center dot
    draw_thick_circle_outline(ctx->renderer, center_x, center_y, 2, 2, ctx->renderer->colors.yellow);

    // Steering angle text
    char steer_str[16];
    snprintf(steer_str, sizeof(steer_str), "%.0f°", steer_angle);
    render_text_small(ctx->renderer, steer_str, center_x - 15, center_y + wheel_radius + 15, ctx->renderer->colors.white);
}

void draw_control_inputs_enhanced(dashboard_context_t *ctx, const vehicle_control_t* controls) {
    SDL_Color control_bg = {30, 30, 30, 255};

    // Throttle bar
    draw_progress_bar(ctx, THROTTLE_BAR_X, CONTROLS_Y, PROGRESS_BAR_WIDTH, PROGRESS_BAR_HEIGHT,
                      controls->accel, 255, "THROTTLE", ctx->renderer->colors.green, control_bg);

    // Brake bar - increase spacing
    draw_progress_bar(ctx, BRAKE_BAR_X, CONTROLS_Y + 30, PROGRESS_BAR_WIDTH, PROGRESS_BAR_HEIGHT,
                      controls->brake, 255, "BRAKE", ctx->renderer->colors.red, control_bg);

    // Steering wheel graphic
    draw_steering_wheel_graphic(ctx, controls->steer);

    // Clutch indicator
    SDL_Color clutch_color = controls->clutch > 128 ? ctx->renderer->colors.yellow : ctx->renderer->colors.dark_gray;
    draw_digital_display(ctx, CLUTCH_INDICATOR_X, CONTROLS_Y, 40, 20, "CLU",
                        (SDL_Color){20, 20, 20, 255}, clutch_color, ctx->renderer->small_font);

    // Handbrake indicator
    SDL_Color handbrake_color = controls->handbrake ? ctx->renderer->colors.red : ctx->renderer->colors.dark_gray;
    draw_digital_display(ctx, HANDBRAKE_INDICATOR_X, CONTROLS_Y, 40, 20, "HBR",
                        (SDL_Color){20, 20, 20, 255}, handbrake_color, ctx->renderer->small_font);
}



// Utility functions
void draw_radial_progress_gauge(dashboard_context_t *ctx, int center_x, int center_y, int radius,
                               float progress, const char* label, SDL_Color progress_color) {
    if (progress < 0) progress = 0;
    if (progress > 1) progress = 1;

    // Draw background circle
    draw_thick_circle_outline(ctx->renderer, center_x, center_y, radius, 2, ctx->renderer->colors.dark_gray);

    // Draw progress arc (270 degrees starting from top)
    float start_angle = 270; // Start at top
    float progress_angle = start_angle + (270 * progress); // 270 degrees max

    draw_arc_thick(ctx->renderer, center_x, center_y, radius - 5, start_angle, progress_angle, 4, progress_color);

    // Draw label
    if (label) {
        render_text_small(ctx->renderer, label, center_x - 10, center_y - 5, ctx->renderer->colors.white);
    }
}

void draw_circular_gauge(dashboard_context_t *ctx, int center_x, int center_y, int radius,
                        float value, float min_val, float max_val,
                        const char* label, const char* unit, SDL_Color gauge_color, SDL_Color needle_color) {
    if (!ctx->renderer) return;

    // Draw outer ring
    draw_thick_circle_outline(ctx->renderer, center_x, center_y, radius, 3, ctx->renderer->colors.white);
    draw_thick_circle_outline(ctx->renderer, center_x, center_y, radius - 10, 2, ctx->renderer->colors.dark_gray);

    // Draw gauge arc (240 degrees, starting from -120 to +120)
    float start_angle = 210; // Start at bottom-left
    float end_angle = 330;   // End at bottom-right
    float total_angle = end_angle - start_angle;

    // Draw background arc
    draw_arc_thick(ctx->renderer, center_x, center_y, radius - 15, start_angle, end_angle, 8, ctx->renderer->colors.dark_gray);

    // Draw value arc
    float value_ratio = (value - min_val) / (max_val - min_val);
    if (value_ratio < 0) value_ratio = 0;
    if (value_ratio > 1) value_ratio = 1;

    float value_angle = start_angle + (total_angle * value_ratio);
    draw_arc_thick(ctx->renderer, center_x, center_y, radius - 15, start_angle, value_angle, 8, gauge_color);

    // Draw tick marks
    for (int i = 0; i <= 10; i++) {
        float tick_angle = start_angle + (total_angle * i / 10.0f);
        float tick_rad = tick_angle * M_PI / 180.0f;

        int tick_length = (i % 5 == 0) ? 15 : 8; // Major/minor ticks
        int outer_x = center_x + (int)((radius - 25) * cos(tick_rad));
        int outer_y = center_y + (int)((radius - 25) * sin(tick_rad));
        int inner_x = center_x + (int)((radius - 25 - tick_length) * cos(tick_rad));
        int inner_y = center_y + (int)((radius - 25 - tick_length) * sin(tick_rad));

        SDL_SetRenderDrawColor(ctx->renderer->renderer, 255, 255, 255, 255);
        SDL_RenderLine(ctx->renderer->renderer, outer_x, outer_y, inner_x, inner_y);

        // Draw numbers for major ticks
        if (i % 5 == 0) {
            float tick_value = min_val + (max_val - min_val) * i / 10.0f;
            char num_str[16];
            snprintf(num_str, sizeof(num_str), "%.0f", tick_value);

            int text_x = center_x + (int)((radius - 45) * cos(tick_rad)) - 10;
            int text_y = center_y + (int)((radius - 45) * sin(tick_rad)) - 8;
            render_text_small(ctx->renderer, num_str, text_x, text_y, ctx->renderer->colors.white);
        }
    }

    // Draw needle
    float needle_angle = start_angle + (total_angle * value_ratio);
    float needle_rad = needle_angle * M_PI / 180.0f;

    // Draw needle shadow
    int shadow_x = center_x + 2;
    int shadow_y = center_y + 2;
    int shadow_end_x = shadow_x + (int)((radius - 30) * cos(needle_rad));
    int shadow_end_y = shadow_y + (int)((radius - 30) * sin(needle_rad));
    SDL_SetRenderDrawColor(ctx->renderer->renderer, 0, 0, 0, 100);
    SDL_RenderLine(ctx->renderer->renderer, shadow_x, shadow_y, shadow_end_x, shadow_end_y);

    // Draw main needle
    int needle_end_x = center_x + (int)((radius - 30) * cos(needle_rad));
    int needle_end_y = center_y + (int)((radius - 30) * sin(needle_rad));
    SDL_SetRenderDrawColor(ctx->renderer->renderer, needle_color.r, needle_color.g, needle_color.b, needle_color.a);

    // Draw thick needle
    for (int i = -2; i <= 2; i++) {
        for (int j = -2; j <= 2; j++) {
            SDL_RenderLine(ctx->renderer->renderer, center_x + i, center_y + j, needle_end_x + i, needle_end_y + j);
        }
    }

    // Draw center circle
    draw_thick_circle_outline(ctx->renderer, center_x, center_y, 8, 8, needle_color);

    // Draw label
    if (label) {
        int label_y = center_y + radius - 25;
        render_text_small(ctx->renderer, label, center_x - 25, label_y, ctx->renderer->colors.white);
    }

    // Draw current value
    char value_str[32];
    snprintf(value_str, sizeof(value_str), "%.1f %s", value, unit ? unit : "");
    int value_y = center_y + 45;
    render_text_large(ctx->renderer, value_str, center_x - 35, value_y, gauge_color);
}

void draw_digital_display(dashboard_context_t *ctx, int x, int y, int width, int height,
                         const char* text, SDL_Color bg_color, SDL_Color text_color, TTF_Font *display_font) {
    if (!ctx->renderer) return;

    // Draw background rectangle
    SDL_FRect bg_rect = {x, y, width, height};
    SDL_SetRenderDrawColor(ctx->renderer->renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
    SDL_RenderFillRect(ctx->renderer->renderer, &bg_rect);

    // Draw border
    SDL_Color border_color = {100, 100, 100, 255};
    SDL_SetRenderDrawColor(ctx->renderer->renderer, border_color.r, border_color.g, border_color.b, border_color.a);
    SDL_RenderRect(ctx->renderer->renderer, &bg_rect);

    // Draw text centered
    if (text && display_font) {
        int text_w, text_h;
        TTF_GetStringSize(display_font, text, 0, &text_w, &text_h);
        int text_x = x + (width - text_w) / 2;
        int text_y = y + (height - text_h) / 2;
        render_text_with_font(ctx->renderer, display_font, text, text_x, text_y, text_color);
    }
}

void draw_gear_indicator(dashboard_context_t *ctx, int x, int y, int gear) {
    SDL_Color bg_color = {20, 20, 20, 255};
    SDL_Color text_color;

    // Color based on gear
    if (gear == 0) {
        text_color = ctx->renderer->colors.white;
    } else if (gear < 0) {
        text_color = (SDL_Color){255, 100, 100, 255}; // Red for reverse
    } else {
        text_color = (SDL_Color){100, 255, 100, 255}; // Green for forward gears
    }

    char gear_str[8];
    if (gear == 0) {
        strcpy(gear_str, "N");
    } else if (gear < 0) {
        strcpy(gear_str, "R");
    } else {
        snprintf(gear_str, sizeof(gear_str), "%d", gear);
    }

    draw_digital_display(ctx, x, y, 80, 80, gear_str, bg_color, text_color, ctx->renderer->large_font);
}

void draw_speed_display(dashboard_context_t *ctx, int x, int y, float speed_kmh) {
    SDL_Color bg_color = {10, 10, 40, 255};
    SDL_Color text_color = ctx->renderer->colors.cyan;

    char speed_str[16];
    snprintf(speed_str, sizeof(speed_str), "%.0f", speed_kmh);

    draw_digital_display(ctx, x, y, 120, 50, speed_str, bg_color, text_color, ctx->renderer->large_font);

    // Add "km/h" label
    SDL_Color label_color = {200, 200, 200, 255};
    render_text_small(ctx->renderer, "km/h", x + 125, y + 15, label_color);
}

void draw_lap_time_display(dashboard_context_t *ctx, int x, int y, float lap_time, const char* label) {
    SDL_Color bg_color = {40, 10, 10, 255};
    SDL_Color text_color = ctx->renderer->colors.yellow;
    SDL_Color label_color = {200, 200, 200, 255};

    // Format time as MM:SS.mmm
    int minutes = (int)(lap_time / 60);
    float seconds = lap_time - (minutes * 60);

    char time_str[16];
    snprintf(time_str, sizeof(time_str), "%d:%06.3f", minutes, seconds);

    draw_digital_display(ctx, x, y, 160, 40, time_str, bg_color, text_color, ctx->renderer->font);

    // Add label
    if (label) {
        render_text_small(ctx->renderer, label, x, y - 20, label_color);
    }
}

void draw_progress_bar(dashboard_context_t *ctx, int x, int y, int width, int height,
                      float value, float max_value, const char* label, SDL_Color bar_color, SDL_Color bg_color) {
    if (!ctx->renderer) return;

    // Draw background
    SDL_FRect bg_rect = {x, y, width, height};
    SDL_SetRenderDrawColor(ctx->renderer->renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
    SDL_RenderFillRect(ctx->renderer->renderer, &bg_rect);

    // Draw border
    SDL_Color border_color = {100, 100, 100, 255};
    SDL_SetRenderDrawColor(ctx->renderer->renderer, border_color.r, border_color.g, border_color.b, border_color.a);
    SDL_RenderRect(ctx->renderer->renderer, &bg_rect);

    // Draw progress
    float progress = value / max_value;
    if (progress < 0) progress = 0;
    if (progress > 1) progress = 1;

    int progress_width = (int)(width * progress);
    if (progress_width > 0) {
        SDL_FRect progress_rect = {x, y, progress_width, height};
        SDL_SetRenderDrawColor(ctx->renderer->renderer, bar_color.r, bar_color.g, bar_color.b, bar_color.a);
        SDL_RenderFillRect(ctx->renderer->renderer, &progress_rect);
    }

    // Draw label and value
    if (label) {
        render_text_small(ctx->renderer, label, x, y - 20, ctx->renderer->colors.white);

        char value_str[16];
        snprintf(value_str, sizeof(value_str), "%.0f%%", progress * 100);
        render_text_small(ctx->renderer, value_str, x + width - 40, y - 20, ctx->renderer->colors.white);
    }
}

void draw_vertical_bar(dashboard_context_t *ctx, int x, int y, int width, int height,
                      float value, float max_value, const char* label, SDL_Color bar_color, SDL_Color bg_color) {
    if (!ctx->renderer) return;

    // Draw background
    SDL_FRect bg_rect = {x, y, width, height};
    SDL_SetRenderDrawColor(ctx->renderer->renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
    SDL_RenderFillRect(ctx->renderer->renderer, &bg_rect);

    // Draw border
    SDL_Color border_color = {100, 100, 100, 255};
    SDL_SetRenderDrawColor(ctx->renderer->renderer, border_color.r, border_color.g, border_color.b, border_color.a);
    SDL_RenderRect(ctx->renderer->renderer, &bg_rect);

    // Draw progress (from bottom)
    float progress = value / max_value;
    if (progress < 0) progress = 0;
    if (progress > 1) progress = 1;

    int progress_height = (int)(height * progress);
    if (progress_height > 0) {
        SDL_FRect progress_rect = {x, y + height - progress_height, width, progress_height};
        SDL_SetRenderDrawColor(ctx->renderer->renderer, bar_color.r, bar_color.g, bar_color.b, bar_color.a);
        SDL_RenderFillRect(ctx->renderer->renderer, &progress_rect);
    }

    // Draw label
    if (label) {
        render_text_small(ctx->renderer, label, x - 5, y + height + 8, ctx->renderer->colors.white);
    }
}

void draw_fuel_level_indicator(dashboard_context_t *ctx, int x, int y, float fuel_percent) {
    SDL_Color bg_color = {40, 40, 40, 255};
    SDL_Color fuel_color;

    // Color based on fuel level
    if (fuel_percent > 50) {
        fuel_color = ctx->renderer->colors.green;
    } else if (fuel_percent > 20) {
        fuel_color = ctx->renderer->colors.yellow;
    } else {
        fuel_color = ctx->renderer->colors.red;
    }

    draw_vertical_bar(ctx, x, y, 30, 100, fuel_percent, 100, "FUEL", fuel_color, bg_color);
}

// Main dashboard render function implementing the comprehensive specification
void dashboard_render(dashboard_context_t *ctx, const telemetry_t *telemetry) {
    if (!ctx->renderer || !telemetry) return;

    // Update flash timer for alerts
    ctx->alert_flash_timer++;

    float speed_kmh = telemetry->speed * 3.6f;
    float fuel_percent = telemetry->fuel * 100.0f;

    // === HEADER SECTION ===
    draw_race_status_led(ctx, telemetry->is_race_on);

    if (telemetry->is_race_on) {
        draw_position_display(ctx, telemetry->race.position);
        draw_lap_counter(ctx, telemetry->race.lap_number, 0); // Total laps not in telemetry
    }

    draw_distance_display(ctx, telemetry->race.distance_traveled / 1000.0f); // Convert to km
    draw_timestamp_display(ctx, telemetry->timestamp_ms);

    // === CENTRAL CLUSTER ===
    draw_speedometer_analog(ctx, speed_kmh);
    draw_tachometer_analog(ctx, &telemetry->engine);
    draw_gear_indicator_enhanced(ctx, telemetry->vehicle_control.gear,
                                telemetry->engine.current_rpm, telemetry->engine.max_rpm);

    // === LAP TIMES WITH DELTAS ===
    if (telemetry->is_race_on) {
        draw_lap_times_with_delta(ctx, &telemetry->race);
    }

    // === LEFT PANEL - Motion Data ===
    draw_acceleration_bars_3axis(ctx, &telemetry->acceleration);
    draw_velocity_vector_3d(ctx, &telemetry->velocity);
    draw_angular_velocity_dials(ctx, &telemetry->angular_velocity);

    // === RIGHT PANEL - Power and Engine ===
    draw_power_gauge_with_peak(ctx, telemetry->power);
    draw_torque_gauge_with_peak(ctx, telemetry->torque);
    draw_boost_gauge_colored(ctx, telemetry->boost);
    draw_fuel_horizontal_bar(ctx, fuel_percent);

    // === BOTTOM SECTION - Wheel Telemetry ===
    draw_wheel_telemetry_section(ctx, telemetry);

    // === DRIVING AIDS ===
    draw_driving_line_gauge(ctx, telemetry->normalized_driving_line);
    draw_ai_brake_difference_bar(ctx, telemetry->normalized_ai_brake_difference);

    // === CONTROL INPUTS ===
    draw_control_inputs_enhanced(ctx, &telemetry->vehicle_control);

    // === ALERTS ===
    draw_object_hit_alerts(ctx, &telemetry->object_hit);

    // === ADDITIONAL INFO ===
    // Car information panel - positioned to avoid overlap with controls
    char car_info[128];
    const char *drivetrain_names[] = {"FWD", "RWD", "AWD"};
    const char *drivetrain = (telemetry->car.drivetrain_type >= 0 && telemetry->car.drivetrain_type <= 2)
        ? drivetrain_names[telemetry->car.drivetrain_type] : "UNK";

    snprintf(car_info, sizeof(car_info), "%s | PI:%d | %dCyl",
             drivetrain, telemetry->car.performance_index, telemetry->car.num_cylinders);

    SDL_Color info_bg = {20, 20, 40, 255};
    draw_digital_display(ctx, 50, WINDOW_HEIGHT - 50, 320, 28, car_info, info_bg,
                        ctx->renderer->colors.cyan, ctx->renderer->small_font);

    // Instructions - positioned to not overlap with other elements
    render_text_small(ctx->renderer, "Press 'Q' or ESC to quit, 'V' to toggle verbose mode",
                      WINDOW_WIDTH - 420, WINDOW_HEIGHT - 20, ctx->renderer->colors.white);
}
