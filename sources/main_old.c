#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <getopt.h>
#include <fcntl.h>
#include <math.h>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "telemetry.h"

// SDL constants
#define WINDOW_WIDTH 1400
#define WINDOW_HEIGHT 900
#define FONT_SIZE 18
#define LARGE_FONT_SIZE 28
#define SMALL_FONT_SIZE 14

// Dashboard layout constants
#define GAUGE_RADIUS 120
#define SMALL_GAUGE_RADIUS 80
#define SPEEDOMETER_X 300
#define SPEEDOMETER_Y 400
#define TACHOMETER_X 700
#define TACHOMETER_Y 400
#define FUEL_GAUGE_X 1100
#define FUEL_GAUGE_Y 300
#define TEMP_GAUGE_X 1100
#define TEMP_GAUGE_Y 500

// Digital display positions
#define GEAR_DISPLAY_X 600
#define GEAR_DISPLAY_Y 350
#define SPEED_DISPLAY_X 300
#define SPEED_DISPLAY_Y 550
#define LAP_TIME_X 50
#define LAP_TIME_Y 50

// Progress bar dimensions
#define PROGRESS_BAR_WIDTH 200
#define PROGRESS_BAR_HEIGHT 20
#define THROTTLE_BAR_X 100
#define THROTTLE_BAR_Y 700
#define BRAKE_BAR_X 100
#define BRAKE_BAR_Y 750

// Mathematical constants
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Legacy constants for backwards compatibility
#define LINE_HEIGHT 30

// SDL globals
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static TTF_Font *font = NULL;
static TTF_Font *large_font = NULL;
static TTF_Font *small_font = NULL;
static int sdl_initialized = 0;
static int quit_requested = 0;

// Forward declarations
void cleanup_sdl(void);

int init_sdl(void)
{
	// Initialize SDL
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
		return -1;
	}

	// Initialize SDL_ttf
	if (!TTF_Init()) {
		fprintf(stderr, "TTF initialization failed: %s\n", SDL_GetError());
		SDL_Quit();
		return -1;
	}

	// Create window
	window = SDL_CreateWindow("Tangue - Racing Telemetry Display",
							  WINDOW_WIDTH, WINDOW_HEIGHT,
							  SDL_WINDOW_RESIZABLE);
	if (!window) {
		fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
		cleanup_sdl();
		return -1;
	}

	// Create renderer
	renderer = SDL_CreateRenderer(window, NULL);
	if (!renderer) {
		fprintf(stderr, "Renderer creation failed: %s\n", SDL_GetError());
		cleanup_sdl();
		return -1;
	}

	// Load font (try to find a system font, fallback to embedded if available)
	const char *font_paths[] = {
		"./assets/fonts/Orbitron/Orbitron-VariableFont_wght.ttf",
		"/System/Library/Fonts/Menlo.ttc",
		"/System/Library/Fonts/Monaco.ttf",
		"/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
		NULL
	};

	// Load fonts in different sizes
	const char *loaded_font_path = NULL;
	for (int i = 0; font_paths[i] != NULL; i++) {
		font = TTF_OpenFont(font_paths[i], FONT_SIZE);
		if (font) {
			loaded_font_path = font_paths[i];
			printf("Loaded font: %s\n", font_paths[i]);
			break;
		}
	}

	if (!font) {
		fprintf(stderr, "Failed to load any font: %s\n", SDL_GetError());
		cleanup_sdl();
		return -1;
	}

	// Load additional font sizes
	large_font = TTF_OpenFont(loaded_font_path, LARGE_FONT_SIZE);
	small_font = TTF_OpenFont(loaded_font_path, SMALL_FONT_SIZE);

	if (!large_font || !small_font) {
		printf("Warning: Could not load all font sizes\n");
	}

	sdl_initialized = 1;
	printf("SDL initialized successfully\n");
	return 0;
}

void cleanup_sdl(void)
{
	if (small_font) {
		TTF_CloseFont(small_font);
		small_font = NULL;
	}
	if (large_font) {
		TTF_CloseFont(large_font);
		large_font = NULL;
	}
	if (font) {
		TTF_CloseFont(font);
		font = NULL;
	}
	if (renderer) {
		SDL_DestroyRenderer(renderer);
		renderer = NULL;
	}
	if (window) {
		SDL_DestroyWindow(window);
		window = NULL;
	}
	if (sdl_initialized) {
		TTF_Quit();
		SDL_Quit();
		sdl_initialized = 0;
	}
}

void render_text_with_font(TTF_Font *font_to_use, const char *text, int x, int y, SDL_Color color)
{
	if (!font_to_use || !renderer) {
		return;
	}

	SDL_Surface *text_surface = TTF_RenderText_Solid(font_to_use, text, 0, color);
	if (!text_surface) {
		fprintf(stderr, "Text surface creation failed: %s\n", SDL_GetError());
		return;
	}

	SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
	if (!text_texture) {
		fprintf(stderr, "Text texture creation failed: %s\n", SDL_GetError());
		SDL_DestroySurface(text_surface);
		return;
	}

	SDL_FRect dst_rect = {x, y, text_surface->w, text_surface->h};
	SDL_RenderTexture(renderer, text_texture, NULL, &dst_rect);

	SDL_DestroyTexture(text_texture);
	SDL_DestroySurface(text_surface);
}

void render_text(const char *text, int x, int y, SDL_Color color)
{
	render_text_with_font(font, text, x, y, color);
}

void draw_circle_outline(int center_x, int center_y, int radius, SDL_Color color)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

	// Draw circle using trigonometry
	for (int angle = 0; angle < 360; angle++) {
		float rad = angle * M_PI / 180.0f;
		int x = center_x + (int)(radius * cos(rad));
		int y = center_y + (int)(radius * sin(rad));
		SDL_RenderPoint(renderer, x, y);
	}
}

void draw_thick_circle_outline(int center_x, int center_y, int radius, int thickness, SDL_Color color)
{
	for (int i = 0; i < thickness; i++) {
		draw_circle_outline(center_x, center_y, radius - i, color);
	}
}

void draw_line_polar(int center_x, int center_y, int radius, float angle_deg, SDL_Color color)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

	float rad = angle_deg * M_PI / 180.0f;
	int end_x = center_x + (int)(radius * cos(rad));
	int end_y = center_y + (int)(radius * sin(rad));

	SDL_RenderLine(renderer, center_x, center_y, end_x, end_y);
}

void draw_arc_thick(int center_x, int center_y, int radius, float start_angle, float end_angle, int thickness, SDL_Color color)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

	for (int t = 0; t < thickness; t++) {
		int current_radius = radius - t;
		for (float angle = start_angle; angle <= end_angle; angle += 1.0f) {
			float rad = angle * M_PI / 180.0f;
			int x = center_x + (int)(current_radius * cos(rad));
			int y = center_y + (int)(current_radius * sin(rad));
			SDL_RenderPoint(renderer, x, y);
		}
	}
}

void draw_circular_gauge(int center_x, int center_y, int radius, float value, float min_val, float max_val,
						 const char* label, const char* unit, SDL_Color gauge_color, SDL_Color needle_color)
{
	// Colors
	SDL_Color white = {255, 255, 255, 255};
	SDL_Color dark_gray = {40, 40, 40, 255};
	SDL_Color light_gray = {120, 120, 120, 255};

	// Draw outer ring
	draw_thick_circle_outline(center_x, center_y, radius, 3, white);
	draw_thick_circle_outline(center_x, center_y, radius - 10, 2, dark_gray);

	// Draw gauge arc (240 degrees, starting from -120 to +120)
	float start_angle = 210; // Start at bottom-left
	float end_angle = 330;   // End at bottom-right
	float total_angle = end_angle - start_angle;

	// Draw background arc
	draw_arc_thick(center_x, center_y, radius - 15, start_angle, end_angle, 8, dark_gray);

	// Draw value arc
	float value_ratio = (value - min_val) / (max_val - min_val);
	if (value_ratio < 0) value_ratio = 0;
	if (value_ratio > 1) value_ratio = 1;

	float value_angle = start_angle + (total_angle * value_ratio);
	draw_arc_thick(center_x, center_y, radius - 15, start_angle, value_angle, 8, gauge_color);

	// Draw tick marks
	for (int i = 0; i <= 10; i++) {
		float tick_angle = start_angle + (total_angle * i / 10.0f);
		float tick_rad = tick_angle * M_PI / 180.0f;

		int tick_length = (i % 5 == 0) ? 15 : 8; // Major/minor ticks
		int outer_x = center_x + (int)((radius - 25) * cos(tick_rad));
		int outer_y = center_y + (int)((radius - 25) * sin(tick_rad));
		int inner_x = center_x + (int)((radius - 25 - tick_length) * cos(tick_rad));
		int inner_y = center_y + (int)((radius - 25 - tick_length) * sin(tick_rad));

		SDL_SetRenderDrawColor(renderer, white.r, white.g, white.b, white.a);
		SDL_RenderLine(renderer, outer_x, outer_y, inner_x, inner_y);

		// Draw numbers for major ticks
		if (i % 5 == 0) {
			float tick_value = min_val + (max_val - min_val) * i / 10.0f;
			char num_str[16];
			snprintf(num_str, sizeof(num_str), "%.0f", tick_value);

			int text_x = center_x + (int)((radius - 45) * cos(tick_rad)) - 10;
			int text_y = center_y + (int)((radius - 45) * sin(tick_rad)) - 8;
			render_text_with_font(small_font ? small_font : font, num_str, text_x, text_y, white);
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
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
	SDL_RenderLine(renderer, shadow_x, shadow_y, shadow_end_x, shadow_end_y);

	// Draw main needle
	int needle_end_x = center_x + (int)((radius - 30) * cos(needle_rad));
	int needle_end_y = center_y + (int)((radius - 30) * sin(needle_rad));
	SDL_SetRenderDrawColor(renderer, needle_color.r, needle_color.g, needle_color.b, needle_color.a);

	// Draw thick needle
	for (int i = -2; i <= 2; i++) {
		for (int j = -2; j <= 2; j++) {
			SDL_RenderLine(renderer, center_x + i, center_y + j, needle_end_x + i, needle_end_y + j);
		}
	}

	// Draw center circle
	draw_thick_circle_outline(center_x, center_y, 8, 8, needle_color);

	// Draw label
	if (label) {
		int label_y = center_y + radius - 30;
		render_text_with_font(small_font ? small_font : font, label, center_x - 30, label_y, white);
	}

	// Draw current value
	char value_str[32];
	snprintf(value_str, sizeof(value_str), "%.1f %s", value, unit ? unit : "");
	int value_y = center_y + 40;
	render_text_with_font(large_font ? large_font : font, value_str, center_x - 40, value_y, gauge_color);
}

void draw_digital_display(int x, int y, int width, int height, const char* text, SDL_Color bg_color, SDL_Color text_color, TTF_Font *display_font)
{
	// Draw background rectangle
	SDL_FRect bg_rect = {x, y, width, height};
	SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
	SDL_RenderFillRect(renderer, &bg_rect);

	// Draw border
	SDL_Color border_color = {100, 100, 100, 255};
	SDL_SetRenderDrawColor(renderer, border_color.r, border_color.g, border_color.b, border_color.a);
	SDL_RenderRect(renderer, &bg_rect);

	// Draw text centered
	if (text && display_font) {
		int text_w, text_h;
		TTF_GetStringSize(display_font, text, 0, &text_w, &text_h);
		int text_x = x + (width - text_w) / 2;
		int text_y = y + (height - text_h) / 2;
		render_text_with_font(display_font, text, text_x, text_y, text_color);
	}
}

void draw_gear_indicator(int x, int y, int gear)
{
	SDL_Color bg_color = {20, 20, 20, 255};
	SDL_Color text_color;

	// Color based on gear
	if (gear == 0) {
		text_color = (SDL_Color){255, 255, 255, 255}; // White for neutral
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

	draw_digital_display(x, y, 80, 80, gear_str, bg_color, text_color, large_font ? large_font : font);
}

void draw_speed_display(int x, int y, float speed_kmh)
{
	SDL_Color bg_color = {10, 10, 40, 255};
	SDL_Color text_color = {0, 255, 255, 255}; // Cyan

	char speed_str[16];
	snprintf(speed_str, sizeof(speed_str), "%.0f", speed_kmh);

	draw_digital_display(x, y, 120, 50, speed_str, bg_color, text_color, large_font ? large_font : font);

	// Add "km/h" label
	SDL_Color label_color = {200, 200, 200, 255};
	render_text_with_font(small_font ? small_font : font, "km/h", x + 125, y + 15, label_color);
}

void draw_lap_time_display(int x, int y, float lap_time, const char* label)
{
	SDL_Color bg_color = {40, 10, 10, 255};
	SDL_Color text_color = {255, 255, 100, 255}; // Yellow
	SDL_Color label_color = {200, 200, 200, 255};

	// Format time as MM:SS.mmm
	int minutes = (int)(lap_time / 60);
	float seconds = lap_time - (minutes * 60);

	char time_str[16];
	snprintf(time_str, sizeof(time_str), "%d:%06.3f", minutes, seconds);

	draw_digital_display(x, y, 160, 40, time_str, bg_color, text_color, font);

	// Add label
	if (label) {
		render_text_with_font(small_font ? small_font : font, label, x, y - 20, label_color);
	}
}

void draw_progress_bar(int x, int y, int width, int height, float value, float max_value,
					   const char* label, SDL_Color bar_color, SDL_Color bg_color)
{
	// Draw background
	SDL_FRect bg_rect = {x, y, width, height};
	SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
	SDL_RenderFillRect(renderer, &bg_rect);

	// Draw border
	SDL_Color border_color = {100, 100, 100, 255};
	SDL_SetRenderDrawColor(renderer, border_color.r, border_color.g, border_color.b, border_color.a);
	SDL_RenderRect(renderer, &bg_rect);

	// Draw progress
	float progress = value / max_value;
	if (progress < 0) progress = 0;
	if (progress > 1) progress = 1;

	int progress_width = (int)(width * progress);
	if (progress_width > 0) {
		SDL_FRect progress_rect = {x, y, progress_width, height};
		SDL_SetRenderDrawColor(renderer, bar_color.r, bar_color.g, bar_color.b, bar_color.a);
		SDL_RenderFillRect(renderer, &progress_rect);
	}

	// Draw label and value
	if (label) {
		SDL_Color text_color = {255, 255, 255, 255};
		render_text_with_font(small_font ? small_font : font, label, x, y - 20, text_color);

		char value_str[16];
		snprintf(value_str, sizeof(value_str), "%.0f%%", progress * 100);
		render_text_with_font(small_font ? small_font : font, value_str, x + width - 40, y - 20, text_color);
	}
}

void draw_vertical_bar(int x, int y, int width, int height, float value, float max_value,
					   const char* label, SDL_Color bar_color, SDL_Color bg_color)
{
	// Draw background
	SDL_FRect bg_rect = {x, y, width, height};
	SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
	SDL_RenderFillRect(renderer, &bg_rect);

	// Draw border
	SDL_Color border_color = {100, 100, 100, 255};
	SDL_SetRenderDrawColor(renderer, border_color.r, border_color.g, border_color.b, border_color.a);
	SDL_RenderRect(renderer, &bg_rect);

	// Draw progress (from bottom)
	float progress = value / max_value;
	if (progress < 0) progress = 0;
	if (progress > 1) progress = 1;

	int progress_height = (int)(height * progress);
	if (progress_height > 0) {
		SDL_FRect progress_rect = {x, y + height - progress_height, width, progress_height};
		SDL_SetRenderDrawColor(renderer, bar_color.r, bar_color.g, bar_color.b, bar_color.a);
		SDL_RenderFillRect(renderer, &progress_rect);
	}

	// Draw label
	if (label) {
		SDL_Color text_color = {255, 255, 255, 255};
		render_text_with_font(small_font ? small_font : font, label, x - 10, y + height + 5, text_color);
	}
}

void draw_fuel_level_indicator(int x, int y, float fuel_percent)
{
	SDL_Color bg_color = {40, 40, 40, 255};
	SDL_Color fuel_color;

	// Color based on fuel level
	if (fuel_percent > 50) {
		fuel_color = (SDL_Color){0, 255, 0, 255}; // Green
	} else if (fuel_percent > 20) {
		fuel_color = (SDL_Color){255, 255, 0, 255}; // Yellow
	} else {
		fuel_color = (SDL_Color){255, 0, 0, 255}; // Red (warning)
	}

	draw_vertical_bar(x, y, 30, 100, fuel_percent, 100, "FUEL", fuel_color, bg_color);
}

void render_telemetry_display(const telemetry_t *telemetry)
{
	if (!renderer || !telemetry) {
		return;
	}

	// Clear the screen with dark racing background
	SDL_SetRenderDrawColor(renderer, 5, 5, 15, 255);
	SDL_RenderClear(renderer);

	// Define colors
	SDL_Color white = {255, 255, 255, 255};
	SDL_Color green = {0, 255, 0, 255};
	SDL_Color red = {255, 0, 0, 255};
	SDL_Color yellow = {255, 255, 0, 255};
	SDL_Color cyan = {0, 255, 255, 255};
	SDL_Color orange = {255, 165, 0, 255};

	// === MAIN GAUGES ===

	// Speedometer (left main gauge)
	float speed_kmh = telemetry->speed * 3.6f;
	draw_circular_gauge(SPEEDOMETER_X, SPEEDOMETER_Y, GAUGE_RADIUS, speed_kmh, 0, 300,
						"SPEED", "km/h", cyan, white);

	// Tachometer (right main gauge)
	float rpm_percent = (telemetry->engine.current_rpm / telemetry->engine.max_rpm) * 100.0f;
	SDL_Color rpm_color = rpm_percent > 80 ? red : (rpm_percent > 60 ? yellow : green);
	draw_circular_gauge(TACHOMETER_X, TACHOMETER_Y, GAUGE_RADIUS, telemetry->engine.current_rpm,
						0, telemetry->engine.max_rpm, "RPM", "rpm", rpm_color, white);

	// Fuel gauge (top right)
	float fuel_percent = telemetry->fuel * 100.0f;
	SDL_Color fuel_color = fuel_percent < 20 ? red : (fuel_percent < 50 ? yellow : green);
	draw_circular_gauge(FUEL_GAUGE_X, FUEL_GAUGE_Y, SMALL_GAUGE_RADIUS, fuel_percent, 0, 100,
						"FUEL", "%", fuel_color, white);

	// Temperature gauge (assume based on power output)
	float temp = 60 + (telemetry->power / 10000); // Simulated temperature
	SDL_Color temp_color = temp > 100 ? red : (temp > 85 ? yellow : green);
	draw_circular_gauge(TEMP_GAUGE_X, TEMP_GAUGE_Y, SMALL_GAUGE_RADIUS, temp, 60, 120,
						"TEMP", "°C", temp_color, white);

	// === DIGITAL DISPLAYS ===

	// Gear indicator (center)
	draw_gear_indicator(GEAR_DISPLAY_X, GEAR_DISPLAY_Y, telemetry->vehicle_control.gear);

	// Speed digital display
	draw_speed_display(SPEED_DISPLAY_X, SPEED_DISPLAY_Y, speed_kmh);

	// Lap times (if in race)
	if (telemetry->is_race_on) {
		draw_lap_time_display(LAP_TIME_X, LAP_TIME_Y, telemetry->race.current_lap, "Current Lap");
		draw_lap_time_display(LAP_TIME_X, LAP_TIME_Y + 50, telemetry->race.best_lap, "Best Lap");
		draw_lap_time_display(LAP_TIME_X, LAP_TIME_Y + 100, telemetry->race.last_lap, "Last Lap");

		// Race position and lap number
		char race_info[64];
		snprintf(race_info, sizeof(race_info), "P%d  LAP %d", telemetry->race.position, telemetry->race.lap_number);
		SDL_Color pos_color = {255, 215, 0, 255}; // Gold
		render_text_with_font(large_font ? large_font : font, race_info, WINDOW_WIDTH - 200, 50, pos_color);
	}

	// === PROGRESS BARS ===

	// Throttle and brake indicators
	SDL_Color throttle_color = {0, 255, 0, 255};
	SDL_Color brake_color = {255, 0, 0, 255};
	SDL_Color control_bg = {30, 30, 30, 255};

	draw_progress_bar(THROTTLE_BAR_X, THROTTLE_BAR_Y, PROGRESS_BAR_WIDTH, PROGRESS_BAR_HEIGHT,
					  telemetry->vehicle_control.accel, 255, "THROTTLE", throttle_color, control_bg);

	draw_progress_bar(BRAKE_BAR_X, BRAKE_BAR_Y, PROGRESS_BAR_WIDTH, PROGRESS_BAR_HEIGHT,
					  telemetry->vehicle_control.brake, 255, "BRAKE", brake_color, control_bg);

	// Clutch and steering
	draw_progress_bar(THROTTLE_BAR_X + 250, THROTTLE_BAR_Y, PROGRESS_BAR_WIDTH, PROGRESS_BAR_HEIGHT,
					  telemetry->vehicle_control.clutch, 255, "CLUTCH", yellow, control_bg);

	// Steering indicator (centered at 128)
	int steering_value = abs(telemetry->vehicle_control.steer - 128);
	SDL_Color steering_color = {255, 165, 0, 255}; // Orange
	draw_progress_bar(THROTTLE_BAR_X + 250, BRAKE_BAR_Y, PROGRESS_BAR_WIDTH, PROGRESS_BAR_HEIGHT,
					  steering_value, 127, "STEERING", steering_color, control_bg);

	// === VERTICAL BARS ===

	// Fuel level vertical indicator
	draw_fuel_level_indicator(WINDOW_WIDTH - 80, WINDOW_HEIGHT - 200, fuel_percent);

	// Power meter
	float power_percent = (telemetry->power / 600000.0f) * 100.0f; // Assuming max 600kW
	SDL_Color power_color = {255, 100, 255, 255}; // Magenta
	SDL_Color power_bg = {20, 20, 20, 255};
	draw_vertical_bar(WINDOW_WIDTH - 120, WINDOW_HEIGHT - 200, 30, 100, power_percent, 100,
					  "PWR", power_color, power_bg);

	// === WARNING LIGHTS ===

	// Handbrake warning
	if (telemetry->vehicle_control.handbrake) {
		SDL_Color warning_bg = {100, 0, 0, 255};
		draw_digital_display(WINDOW_WIDTH - 150, WINDOW_HEIGHT - 100, 80, 25, "HANDBRAKE", warning_bg, white, small_font);
	}

	// Low fuel warning
	if (fuel_percent < 15) {
		SDL_Color warning_bg = {100, 0, 0, 255};
		draw_digital_display(WINDOW_WIDTH - 150, WINDOW_HEIGHT - 70, 80, 25, "LOW FUEL", warning_bg, white, small_font);
	}

	// === ADDITIONAL INFO ===

	// Car information panel
	char car_info[128];
	const char *drivetrain_names[] = {"FWD", "RWD", "AWD"};
	const char *drivetrain = (telemetry->car.drivetrain_type >= 0 && telemetry->car.drivetrain_type <= 2)
		? drivetrain_names[telemetry->car.drivetrain_type] : "UNK";

	snprintf(car_info, sizeof(car_info), "%s | PI:%d | %dCyl",
			 drivetrain, telemetry->car.performance_index, telemetry->car.num_cylinders);

	SDL_Color info_bg = {20, 20, 40, 255};
	draw_digital_display(50, WINDOW_HEIGHT - 100, 300, 25, car_info, info_bg, cyan, small_font);

	// Power and torque display
	char power_info[64];
	snprintf(power_info, sizeof(power_info), "%.0f HP | %.0f Nm", telemetry->power * 0.00134102, telemetry->torque);
	draw_digital_display(50, WINDOW_HEIGHT - 70, 200, 25, power_info, info_bg, orange, small_font);

	// Instructions
	render_text_with_font(small_font ? small_font : font,
						  "Press 'Q' or ESC to quit, 'V' to toggle verbose mode",
						  50, WINDOW_HEIGHT - 30, white);

	// Present the rendered frame
	SDL_RenderPresent(renderer);
}

int handle_sdl_events(int *verbose)
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_EVENT_QUIT:
				quit_requested = 1;
				return 1;
			case SDL_EVENT_KEY_DOWN:
				switch (event.key.key) {
					case SDLK_Q:
					case SDLK_ESCAPE:
						quit_requested = 1;
						return 1;
					case SDLK_V:
						*verbose = !(*verbose);
						printf("Verbose mode %s\n", *verbose ? "enabled" : "disabled");
						break;
				}
				break;
		}
	}
	return 0;
}

void print_usage(const char *program_name)
{
	printf("Usage: %s [OPTIONS]\n", program_name);
	printf("UDP server for receiving and parsing telemetry packets\n\n");
	printf("Options:\n");
	printf("  -p, --port PORT     Port to listen on (required, 1-65535)\n");
	printf("  -v, --verbose       Enable verbose output\n");
	printf("  -h, --help         Show this help message\n\n");
	printf("Examples:\n");
	printf("  %s -p 12345\n", program_name);
	printf("  %s --port 8080 --verbose\n", program_name);
}

void print_telemetry_info(const telemetry_t *telemetry)
{
	printf("=== Telemetry Info ===\n");
	printf("Race On: %s\n", telemetry->is_race_on ? "Yes" : "No");
	printf("Timestamp: %u ms\n", telemetry->timestamp_ms);
	printf("Speed: %.2f m/s (%.2f km/h)\n", telemetry->speed, telemetry->speed * 3.6f);
	printf("Engine RPM: %.0f / %.0f\n", telemetry->engine.current_rpm, telemetry->engine.max_rpm);
	printf("Position: (%.2f, %.2f, %.2f)\n", telemetry->position.x, telemetry->position.y, telemetry->position.z);
	printf("Gear: %d\n", telemetry->vehicle_control.gear);
	printf("Fuel: %.1f%%\n", telemetry->fuel * 100.0f);
	if (telemetry->is_race_on) {
		printf("Lap: %d\n", telemetry->race.lap_number);
		printf("Position: %d\n", telemetry->race.position);
		printf("Current Lap Time: %.3f s\n", telemetry->race.current_lap);
		printf("Best Lap: %.3f s\n", telemetry->race.best_lap);
	}
	printf("==================\n\n");
}

int parse_packet(const unsigned char *buffer, size_t buffer_size, telemetry_t *telemetry)
{
	if (buffer_size < sizeof(telemetry_t)) {
		fprintf(stderr, "Buffer too small: %zu bytes, expected at least %zu bytes\n",
				buffer_size, sizeof(telemetry_t));
		return -1;
	}

	// Direct memory copy (assuming same endianness)
	memcpy(telemetry, buffer, sizeof(telemetry_t));
	return 0;
}

int create_udp_server(int port)
{
	int sockfd;
	struct sockaddr_in server_addr;

	// Create UDP socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		perror("Socket creation failed");
		return -1;
	}

	// Allow address reuse
	int opt = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		perror("setsockopt failed");
		close(sockfd);
		return -1;
	}

	// Configure server address
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	// Bind socket to address
	if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("Bind failed");
		close(sockfd);
		return -1;
	}

	printf("UDP server listening on port %d\n", port);
	return sockfd;
}

void run_server(int sockfd, int verbose)
{
	unsigned char buffer[2048];
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	telemetry_t telemetry;
	ssize_t bytes_received;
	int have_telemetry_data = 0;

	// Set socket to non-blocking for integration with SDL event loop
	int flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

	printf("Server running... Press 'Q' or ESC to quit, 'V' to toggle verbose mode\n\n");

	// Initialize telemetry with default values for display
	memset(&telemetry, 0, sizeof(telemetry));

	while (!quit_requested) {
		// Handle SDL events
		if (handle_sdl_events(&verbose)) {
			break;
		}

		// Try to receive data (non-blocking)
		bytes_received = recvfrom(sockfd, buffer, sizeof(buffer), 0,
								(struct sockaddr *)&client_addr, &client_len);

		if (bytes_received > 0) {
			if (verbose) {
				printf("Received %zd bytes from %s:%d\n",
					   bytes_received, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
			}

			// Parse the packet
			if (parse_packet(buffer, bytes_received, &telemetry) == 0) {
				have_telemetry_data = 1;
				if (verbose) {
					print_telemetry_info(&telemetry);
				} else {
					// Brief output in non-verbose mode
					printf("Packet received - Speed: %.1f km/h, RPM: %.0f, Gear: %d\n",
						   telemetry.speed * 3.6f, telemetry.engine.current_rpm, telemetry.vehicle_control.gear);
				}
			} else {
				printf("Failed to parse packet\n\n");
			}
		} else if (bytes_received < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
			if (errno == EINTR) {
				continue; // Interrupted by signal, continue
			}
			perror("recvfrom failed");
			break;
		}

		// Render telemetry display
		if (sdl_initialized) {
			if (have_telemetry_data) {
				render_telemetry_display(&telemetry);
			} else {
				// Show waiting screen
				SDL_SetRenderDrawColor(renderer, 15, 15, 25, 255);
				SDL_RenderClear(renderer);
				SDL_Color white = {255, 255, 255, 255};
				SDL_Color cyan = {0, 255, 255, 255};
				render_text("TANGUE - Racing Telemetry Display", 300, 300, cyan);
				render_text("Waiting for telemetry data...", 350, 350, white);
				render_text("Press 'Q' or ESC to quit", 380, 400, white);
				SDL_RenderPresent(renderer);
			}
		}

		// Small delay to prevent excessive CPU usage
		SDL_Delay(16); // ~60 FPS
	}
}

int main(const int argc, const char *argv[], const char *envp[])
{
	int port = -1;
	int verbose = 0;
	int sockfd;
	int opt;

	// Define long options
	static struct option long_options[] = {
		{"port", required_argument, 0, 'p'},
		{"verbose", no_argument, 0, 'v'},
		{"help", no_argument, 0, 'h'},
		{0, 0, 0, 0}
	};

	// Parse command line arguments
	while ((opt = getopt_long(argc, (char *const *)argv, "p:vh", long_options, NULL)) != -1) {
		switch (opt) {
			case 'p':
				port = atoi(optarg);
				if (port <= 0 || port > 65535) {
					fprintf(stderr, "Error: Port must be between 1 and 65535\n");
					return EXIT_FAILURE;
				}
				break;
			case 'v':
				verbose = 1;
				break;
			case 'h':
				print_usage(argv[0]);
				return EXIT_SUCCESS;
			case '?':
				// getopt_long already printed error message
				print_usage(argv[0]);
				return EXIT_FAILURE;
			default:
				fprintf(stderr, "Unexpected option: %c\n", opt);
				print_usage(argv[0]);
				return EXIT_FAILURE;
		}
	}

	// Check if port was specified
	if (port == -1) {
		fprintf(stderr, "Error: Port is required\n\n");
		print_usage(argv[0]);
		return EXIT_FAILURE;
	}

	if (verbose) {
		printf("Starting UDP server with verbose output enabled\n");
	}

	// Initialize SDL
	if (init_sdl() < 0) {
		return EXIT_FAILURE;
	}

	// Create UDP server
	sockfd = create_udp_server(port);
	if (sockfd < 0) {
		cleanup_sdl();
		return EXIT_FAILURE;
	}

	// Run the server
	run_server(sockfd, verbose);

	// Cleanup
	close(sockfd);
	cleanup_sdl();
	return EXIT_SUCCESS;
}
