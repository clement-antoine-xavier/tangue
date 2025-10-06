#pragma once

#include "renderer.h"
#include "telemetry.h"

// Dashboard layout constants - Header Section
#define HEADER_HEIGHT 100
#define RACE_STATUS_LED_X 30
#define RACE_STATUS_LED_Y 25
#define RACE_STATUS_LED_SIZE 20
#define POSITION_DISPLAY_X (WINDOW_WIDTH/2 - 100)
#define POSITION_DISPLAY_Y 20
#define LAP_COUNTER_X (WINDOW_WIDTH/2 + 20)
#define LAP_COUNTER_Y 20

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

// Central cluster - Main gauges
#define GAUGE_RADIUS 100
#define SMALL_GAUGE_RADIUS 60
#define MINI_GAUGE_RADIUS 35
#define SPEEDOMETER_X (WINDOW_WIDTH/2)
#define SPEEDOMETER_Y 350
#define TACHOMETER_X (WINDOW_WIDTH/2 - 250)
#define TACHOMETER_Y 350
#define GEAR_DISPLAY_X (WINDOW_WIDTH/2 + 200)  // Déplacé plus à droite
#define GEAR_DISPLAY_Y 310

// Left panel - Acceleration, velocity, angular velocity
#define LEFT_PANEL_X 50
#define LEFT_PANEL_Y 160
#define ACCELERATION_BARS_X LEFT_PANEL_X
#define ACCELERATION_BARS_Y LEFT_PANEL_Y
#define VELOCITY_ARROW_X LEFT_PANEL_X
#define VELOCITY_ARROW_Y (LEFT_PANEL_Y + 140)
#define ANGULAR_VELOCITY_X LEFT_PANEL_X
#define ANGULAR_VELOCITY_Y (LEFT_PANEL_Y + 280)

// Right panel - Power, torque, boost, fuel
#define RIGHT_PANEL_X (WINDOW_WIDTH - 300)  // Espace supplémentaire
#define RIGHT_PANEL_Y 150
#define POWER_GAUGE_X RIGHT_PANEL_X
#define POWER_GAUGE_Y RIGHT_PANEL_Y
#define TORQUE_GAUGE_X RIGHT_PANEL_X
#define TORQUE_GAUGE_Y (RIGHT_PANEL_Y + 140)
#define BOOST_GAUGE_X (RIGHT_PANEL_X + 150)  // Espacement accru
#define BOOST_GAUGE_Y RIGHT_PANEL_Y
#define FUEL_BAR_X (RIGHT_PANEL_X)
#define FUEL_BAR_Y (RIGHT_PANEL_Y + 280)

// Lap time displays
#define LAP_TIME_PANEL_X 50
#define LAP_TIME_PANEL_Y 110
#define LAP_TIME_WIDTH 220
#define LAP_TIME_HEIGHT 28

// Bottom section - Wheel telemetry
#define WHEEL_SECTION_Y (WINDOW_HEIGHT - 220)
#define WHEEL_FL_X 120
#define WHEEL_FR_X 340
#define WHEEL_RL_X 560
#define WHEEL_RR_X 780
#define WHEEL_BAR_WIDTH 18
#define WHEEL_BAR_HEIGHT 85
#define WHEEL_GAUGE_RADIUS 28

// Control inputs
#define CONTROLS_Y (WINDOW_HEIGHT - 120)
#define THROTTLE_BAR_X 50
#define BRAKE_BAR_X 50
#define STEERING_DISPLAY_X 320
#define CLUTCH_INDICATOR_X 420
#define HANDBRAKE_INDICATOR_X 480

// Progress bar dimensions
#define PROGRESS_BAR_WIDTH 150
#define PROGRESS_BAR_HEIGHT 20
#define VERTICAL_BAR_WIDTH 20
#define VERTICAL_BAR_HEIGHT 100

// Alert system
#define ALERT_BOX_X (WINDOW_WIDTH/2 - 150)
#define ALERT_BOX_Y 150  // Descendu pour éviter le header
#define ALERT_BOX_WIDTH 300
#define ALERT_BOX_HEIGHT 50

// Dashboard context structure with state tracking
typedef struct {
    renderer_context_t *renderer;
    float previous_lap_time;
    float best_personal_lap;
    int alert_flash_timer;
    float power_peak_hold;
    float torque_peak_hold;
} dashboard_context_t;

// Main dashboard functions
int dashboard_init(dashboard_context_t *ctx, renderer_context_t *renderer);
void dashboard_cleanup(dashboard_context_t *ctx);
void dashboard_render(dashboard_context_t *ctx, const telemetry_t *telemetry);

// Header section functions
void draw_race_status_led(dashboard_context_t *ctx, int is_race_on);
void draw_position_display(dashboard_context_t *ctx, int position);
void draw_lap_counter(dashboard_context_t *ctx, int current_lap, int total_laps);
void draw_distance_display(dashboard_context_t *ctx, float distance_km);
void draw_timestamp_display(dashboard_context_t *ctx, unsigned int timestamp_ms);

// Central cluster functions
void draw_speedometer_analog(dashboard_context_t *ctx, float speed_kmh);
void draw_tachometer_analog(dashboard_context_t *ctx, const engine_t* engine);
void draw_gear_indicator_enhanced(dashboard_context_t *ctx, int gear, float rpm, float max_rpm);

// Lap time functions with delta calculations
void draw_lap_times_with_delta(dashboard_context_t *ctx, const race_t* race);
void calculate_lap_delta(float current_time, float reference_time, char* delta_str, SDL_Color* color);

// Acceleration and velocity displays
void draw_acceleration_bars_3axis(dashboard_context_t *ctx, const vector3_t* acceleration);
void draw_velocity_vector_3d(dashboard_context_t *ctx, const vector3_t* velocity);
void draw_angular_velocity_dials(dashboard_context_t *ctx, const vector3_t* angular_velocity);

// Wheel telemetry section
void draw_wheel_telemetry_section(dashboard_context_t *ctx, const telemetry_t* telemetry);
void draw_suspension_travel_bars(dashboard_context_t *ctx, const wheel_values_t* suspension);
void draw_tire_slip_gauges(dashboard_context_t *ctx, const wheel_values_t* slip_ratio, const wheel_values_t* slip_angle);
void draw_tire_temperature_bars(dashboard_context_t *ctx, const wheel_values_t* tire_temps);
void draw_wheel_surface_indicators(dashboard_context_t *ctx, const wheel_values_t* rumble,
                                  const wheel_values_t* puddle, const wheel_values_t* rumble_strip);

// Power and torque with peak hold
void draw_power_gauge_with_peak(dashboard_context_t *ctx, float power_watts);
void draw_torque_gauge_with_peak(dashboard_context_t *ctx, float torque_nm);
void draw_boost_gauge_colored(dashboard_context_t *ctx, float boost_psi);

// Enhanced fuel display
void draw_fuel_horizontal_bar(dashboard_context_t *ctx, float fuel_percent);

// Driving aids displays
void draw_driving_line_gauge(dashboard_context_t *ctx, unsigned char driving_line_accuracy);
void draw_ai_brake_difference_bar(dashboard_context_t *ctx, unsigned char ai_brake_diff);

// Alert system
void draw_object_hit_alerts(dashboard_context_t *ctx, const object_hit_t* object_hits);

// Enhanced control inputs
void draw_control_inputs_enhanced(dashboard_context_t *ctx, const vehicle_control_t* controls);
void draw_steering_wheel_graphic(dashboard_context_t *ctx, unsigned char steer_value);

// Utility gauge functions
void draw_circular_gauge(dashboard_context_t *ctx, int center_x, int center_y, int radius,
                        float value, float min_val, float max_val,
                        const char* label, const char* unit, SDL_Color gauge_color, SDL_Color needle_color);
void draw_radial_progress_gauge(dashboard_context_t *ctx, int center_x, int center_y, int radius,
                               float progress, const char* label, SDL_Color progress_color);
void draw_digital_display(dashboard_context_t *ctx, int x, int y, int width, int height,
                         const char* text, SDL_Color bg_color, SDL_Color text_color, TTF_Font *display_font);
void draw_progress_bar(dashboard_context_t *ctx, int x, int y, int width, int height,
                      float value, float max_value, const char* label, SDL_Color bar_color, SDL_Color bg_color);
void draw_vertical_bar(dashboard_context_t *ctx, int x, int y, int width, int height,
                      float value, float max_value, const char* label, SDL_Color bar_color, SDL_Color bg_color);
