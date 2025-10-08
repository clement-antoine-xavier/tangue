#pragma once

#include <stddef.h>

/* Scalar wrappers */
typedef struct
{
	int flag;
} is_race_on_t;
typedef struct
{
	unsigned int ms;
} timestamp_t;
typedef struct
{
	float value;
} speed_t;
typedef struct
{
	float value;
} power_t;
typedef struct
{
	float value;
} torque_t;
typedef struct
{
	float value;
} boost_t;
typedef struct
{
	float percent;
} fuel_t;
typedef struct
{
	unsigned char value;
} normalized_driving_line_t;
typedef struct
{
	unsigned char value;
} normalized_ai_brake_difference_t;

/* Engine */
typedef struct engine_s
{
	float max_rpm;
	float idle_rpm;
	float current_rpm;
} engine_t;

/* Orientation and vectors (no reuse) */
typedef struct acceleration_s
{
	float x;
	float y;
	float z;
} acceleration_t;
typedef struct velocity_s
{
	float x;
	float y;
	float z;
} velocity_t;
typedef struct angular_velocity_s
{
	float x;
	float y;
	float z;
} angular_velocity_t;
typedef struct rotation_s
{
	float yaw;
	float pitch;
	float roll;
} rotation_t;
typedef struct position_s
{
	float x;
	float y;
	float z;
} position_t;

/* Per-wheel groups (no reuse) */
typedef struct normalized_suspension_travel_s
{
	float front_left;
	float front_right;
	float rear_left;
	float rear_right;
} normalized_suspension_travel_t;
typedef struct tire_slip_ratio_s
{
	float front_left;
	float front_right;
	float rear_left;
	float rear_right;
} tire_slip_ratio_t;
typedef struct wheel_rotation_speed_s
{
	float front_left;
	float front_right;
	float rear_left;
	float rear_right;
} wheel_rotation_speed_t;
typedef struct wheel_on_rumble_strip_s
{
	float front_left;
	float front_right;
	float rear_left;
	float rear_right;
} wheel_on_rumble_strip_t;
typedef struct wheel_in_puddle_depth_s
{
	float front_left;
	float front_right;
	float rear_left;
	float rear_right;
} wheel_in_puddle_depth_t;
typedef struct surface_rumble_s
{
	float front_left;
	float front_right;
	float rear_left;
	float rear_right;
} surface_rumble_t;
typedef struct tire_slip_angle_s
{
	float front_left;
	float front_right;
	float rear_left;
	float rear_right;
} tire_slip_angle_t;
typedef struct tire_combined_slip_s
{
	float front_left;
	float front_right;
	float rear_left;
	float rear_right;
} tire_combined_slip_t;
typedef struct suspension_travel_meters_s
{
	float front_left;
	float front_right;
	float rear_left;
	float rear_right;
} suspension_travel_meters_t;
typedef struct tire_temp_s
{
	float front_left;
	float front_right;
	float rear_left;
	float rear_right;
} tire_temp_t;

/* Car, race, controls */
typedef struct car_s
{
	int ordinal;
	int classification;	   /* 0..7 */
	int performance_index; /* 100..999 */
	int drivetrain_type;   /* 0=FWD,1=RWD,2=AWD */
	int num_cylinders;
	int type;
} car_t;

typedef struct
{
	char obj[6]; /* objects hit (up to 6) */
} object_hit_t;

typedef struct race_s
{
	float distance_traveled;
	float best_lap;
	float last_lap;
	float current_lap;
	float current_time;
	short lap_number;
	unsigned char position;
} race_t;

typedef struct vehicle_control_s
{
	unsigned char accel;
	unsigned char brake;
	unsigned char clutch;
	unsigned char handbrake;
	unsigned char gear;
	unsigned char steer;
} vehicle_control_t;

/* Root telemetry */
typedef struct telemetry_s
{
	is_race_on_t is_race_on;
	timestamp_t timestamp;
	engine_t engine;

	acceleration_t acceleration;
	velocity_t velocity;
	angular_velocity_t angular_velocity;
	rotation_t rotation;

	normalized_suspension_travel_t normalized_suspension_travel;
	tire_slip_ratio_t tire_slip_ratio;
	wheel_rotation_speed_t wheel_rotation_speed;
	wheel_on_rumble_strip_t wheel_on_rumble_strip;
	wheel_in_puddle_depth_t wheel_in_puddle_depth;
	surface_rumble_t surface_rumble;
	tire_slip_angle_t tire_slip_angle;
	tire_combined_slip_t tire_combined_slip;
	suspension_travel_meters_t suspension_travel_meters;

	car_t car;
	object_hit_t object_hit;

	position_t position;
	speed_t speed;
	power_t power;
	torque_t torque;
	tire_temp_t tire_temp;
	boost_t boost;
	fuel_t fuel;
	race_t race;
	vehicle_control_t vehicle_control;

	normalized_driving_line_t normalized_driving_line;
	normalized_ai_brake_difference_t normalized_ai_brake_difference;
} telemetry_t;

void print_is_race_on(const is_race_on_t *is_race_on);
void print_timestamp(const timestamp_t *timestamp);

void print_engine(const engine_t *engine);

void print_acceleration(const acceleration_t *acceleration);
void print_velocity(const velocity_t *velocity);
void print_angular_velocity(const angular_velocity_t *angular_velocity);
void print_rotation(const rotation_t *rotation);
void print_position(const position_t *position);

void print_normalized_suspension_travel(const normalized_suspension_travel_t *normalized_suspension_travel);
void print_tire_slip_ratio(const tire_slip_ratio_t *tire_slip_ratio);
void print_wheel_rotation_speed(const wheel_rotation_speed_t *wheel_rotation_speed);
void print_wheel_on_rumble_strip(const wheel_on_rumble_strip_t *wheel_on_rumble_strip);
void print_wheel_in_puddle_depth(const wheel_in_puddle_depth_t *wheel_in_puddle_depth);
void print_surface_rumble(const surface_rumble_t *surface_rumble);
void print_tire_slip_angle(const tire_slip_angle_t *tire_slip_angle);
void print_tire_combined_slip(const tire_combined_slip_t *tire_combined_slip);
void print_suspension_travel_meters(const suspension_travel_meters_t *suspension_travel_meters);
void print_tire_temp(const tire_temp_t *tire_temp);

void print_car(const car_t *car);
void print_object_hit(const object_hit_t *object_hit);
void print_race(const race_t *race);
void print_vehicle_control(const vehicle_control_t *vehicle_control);

void print_speed(const speed_t *speed);
void print_power(const power_t *power);
void print_torque(const torque_t *torque);
void print_boost(const boost_t *boost);
void print_fuel(const fuel_t *fuel);
void print_normalized_driving_line(const normalized_driving_line_t *normalized_driving_line);
void print_normalized_ai_brake_difference(const normalized_ai_brake_difference_t *normalized_ai_brake_difference);

void print_telemetry(const telemetry_t *telemetry);
