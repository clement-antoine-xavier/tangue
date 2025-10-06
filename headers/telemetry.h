#pragma once

typedef struct engine_s
{
	float max_rpm;
	float idle_rpm;
	float current_rpm;
} engine_t;

typedef struct vector3_s
{
	float x; // Right
	float y; // Up
	float z; // Forward
} vector3_t;

typedef struct rotation_s
{
	float yaw;
	float pitch;
	float roll;
} rotation_t;

typedef struct wheel_values_s
{
	float front_left;
	float front_right;
	float rear_left;
	float rear_right;
} wheel_values_t;

typedef struct car_s
{
	int ordinal;		   // Unique ID of the car make/model
	int classification;	   // Between 0 (D - lowest) and 7 (X class - highest) inclusive
	int performance_index; // Between 100 (lowest) and 999 (highest) inclusive
	int drivetrain_type;   // 0 = FWD, 1 = RWD, 2 = AWD
	int num_cylinders;	   // Number of cylinders in the engine
	int type;			   // Unique ID of the car make/model
} car_t;

typedef struct
{
	char obj[6]; // Objects hit (up to 6)
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

typedef struct telemetry_s
{
	int is_race_on;			   // 1 when race is on, 0 otherwise
	unsigned int timestamp_ms; // Current in-game timestamp
	engine_t engine;
	vector3_t acceleration;	  // X/Y/Z acceleration
	vector3_t velocity;		  // X/Y/Z velocity
	vector3_t angular_velocity; // X/Y/Z angular velocity
	rotation_t rotation;
	wheel_values_t normalized_suspension_travel; // 0.0f = max stretch; 1.0 = max compression
	wheel_values_t tire_slip_ratio;			   // Normalized slip ratio
	wheel_values_t wheel_rotation_speed;		   // Wheel rotation speed (rad/sec)
	wheel_values_t wheel_on_rumble_strip;		   // 1 = on rumble strip, 0 = off
	wheel_values_t wheel_in_puddle_depth;		   // 0 to 1, 1 = deepest puddle
	wheel_values_t surface_rumble;
	wheel_values_t tire_slip_angle;
	wheel_values_t tire_combined_slip;
	wheel_values_t suspension_travel_meters; // Suspension travel in meters
	car_t car;
	object_hit_t object_hit;
	vector3_t position;		// Vehicle position in the world
	float speed;			// Speed (m/s)
	float power;			// Power (watts)
	float torque;			// Torque (newton-meter)
	wheel_values_t tire_temp; // Tire temperature (F)
	float boost;			// Boost (PSI)
	float fuel;				// % of fuel left
	race_t race;
	vehicle_control_t vehicle_control;
	unsigned char normalized_driving_line; // Driving line follow accuracy
	unsigned char normalized_ai_brake_difference;
} telemetry_t;
