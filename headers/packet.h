/**
 * @file packet.h
 * @brief Telemetry and vehicle state packet definitions for a racing simulation.
 *
 * This header defines strongly-typed structures used to represent engine,
 * vehicle dynamics, race state, controller inputs, and aggregated telemetry
 * for a single update "packet".
 */

/**
 * @brief Engine telemetry values.
 * @struct engine_s
 * @typedef engine_t
 *
 * @var engine_s::max_rpm
 *   Maximum engine speed in revolutions per minute (RPM).
 * @var engine_s::idle_rpm
 *   Engine idle speed in RPM.
 * @var engine_s::current_rpm
 *   Current engine speed in RPM.
 */

/**
 * @brief 3D vector with a right-handed coordinate system.
 * @struct vector3_s
 * @typedef vector3_t
 *
 * @var vector3_s::x
 *   Right (+X) axis component.
 * @var vector3_s::y
 *   Up (+Y) axis component.
 * @var vector3_s::z
 *   Forward (+Z) axis component.
 */

/**
 * @brief Vehicle orientation using Euler angles.
 * @struct rotation_s
 * @typedef rotation_t
 *
 * @var rotation_s::yaw
 *   Rotation around the up (Y) axis.
 * @var rotation_s::pitch
 *   Rotation around the right (X) axis.
 * @var rotation_s::roll
 *   Rotation around the forward (Z) axis.
 */

/**
 * @brief Values provided per wheel, ordered: front-left, front-right, rear-left, rear-right.
 * @struct wheel_values_s
 * @typedef wheel_values_t
 *
 * @var wheel_values_s::front_left
 *   Value for the front-left wheel.
 * @var wheel_values_s::front_right
 *   Value for the front-right wheel.
 * @var wheel_values_s::rear_left
 *   Value for the rear-left wheel.
 * @var wheel_values_s::rear_right
 *   Value for the rear-right wheel.
 */

/**
 * @brief Car metadata and configuration.
 * @struct car_s
 * @typedef car_t
 *
 * @var car_s::ordinal
 *   Unique ID of the car make/model.
 * @var car_s::classification
 *   Performance class from 0 (D - lowest) to 7 (X - highest), inclusive.
 * @var car_s::performance_index
 *   Performance index from 100 (lowest) to 999 (highest), inclusive.
 * @var car_s::drivetrain_type
 *   Drivetrain type: 0 = FWD, 1 = RWD, 2 = AWD.
 * @var car_s::num_cylinders
 *   Number of engine cylinders.
 * @var car_s::type
 *   Unique ID of the car make/model (may mirror or complement ordinal).
 */

/**
 * @brief Objects hit by the vehicle during the current interval.
 * @typedef object_hit_t
 *
 * @var object_hit_t::obj
 *   Up to 6 object identifiers for objects hit.
 */

/**
 * @brief Race timing and position information.
 * @struct race_s
 * @typedef race_t
 *
 * @var race_s::distance_traveled
 *   Total distance traveled.
 * @var race_s::best_lap
 *   Best lap time.
 * @var race_s::last_lap
 *   Previous lap time.
 * @var race_s::current_lap
 *   Current lap time so far.
 * @var race_s::current_time
 *   Session time at the moment of capture.
 * @var race_s::lap_number
 *   Current lap number.
 * @var race_s::position
 *   Current race position (1 = leader).
 */

/**
 * @brief Driver input and control states.
 * @struct vehicle_control_s
 * @typedef vehicle_control_t
 *
 * @var vehicle_control_s::accel
 *   Accelerator input.
 * @var vehicle_control_s::brake
 *   Brake input.
 * @var vehicle_control_s::clutch
 *   Clutch input.
 * @var vehicle_control_s::handbrake
 *   Handbrake input.
 * @var vehicle_control_s::gear
 *   Current gear selection.
 * @var vehicle_control_s::steer
 *   Steering input.
 */

/**
 * @brief Complete telemetry packet for a single update.
 * @struct packet_s
 * @typedef packet_t
 *
 * @var packet_s::is_race_on
 *   Race state flag (1 = race active, 0 = not in race).
 * @var packet_s::timestamp_ms
 *   In-game timestamp in milliseconds.
 * @var packet_s::engine
 *   Engine telemetry values.
 * @var packet_s::acceleration
 *   Linear acceleration vector (X/Y/Z).
 * @var packet_s::velocity
 *   Linear velocity vector (X/Y/Z).
 * @var packet_s::angular_velocity
 *   Angular velocity vector (X/Y/Z).
 * @var packet_s::rotation
 *   Vehicle orientation (yaw, pitch, roll).
 * @var packet_s::normalized_suspension_travel
 *   Normalized suspension travel per wheel; 0.0 = max stretch, 1.0 = max compression.
 * @var packet_s::tire_slip_ratio
 *   Normalized slip ratio per wheel.
 * @var packet_s::wheel_rotation_speed
 *   Wheel rotational speed per wheel (radians/second).
 * @var packet_s::wheel_on_rumble_strip
 *   Per-wheel flag indicating contact with rumble strip (1 = on, 0 = off).
 * @var packet_s::wheel_in_puddle_depth
 *   Water depth per wheel; 0.0 to 1.0 where 1.0 = deepest puddle.
 * @var packet_s::surface_rumble
 *   Surface rumble intensity per wheel.
 * @var packet_s::tire_slip_angle
 *   Tire slip angle per wheel.
 * @var packet_s::tire_combined_slip
 *   Combined tire slip per wheel.
 * @var packet_s::suspension_travel_meters
 *   Suspension travel per wheel (meters).
 * @var packet_s::car
 *   Car metadata and configuration.
 * @var packet_s::object_hit
 *   Objects hit during this interval.
 * @var packet_s::position
 *   World position of the vehicle (X/Y/Z).
 * @var packet_s::speed
 *   Vehicle speed (meters/second).
 * @var packet_s::power
 *   Engine power output (watts).
 * @var packet_s::torque
 *   Engine torque (newton-meter).
 * @var packet_s::tire_temp
 *   Tire temperature per wheel (degrees Fahrenheit).
 * @var packet_s::boost
 *   Boost pressure (PSI).
 * @var packet_s::fuel
 *   Remaining fuel as a percentage (0.0 to 100.0).
 * @var packet_s::race
 *   Race timing and position data.
 * @var packet_s::vehicle_control
 *   Driver input/control states.
 * @var packet_s::normalized_driving_line
 *   Driving line follow accuracy (normalized).
 * @var packet_s::normalized_ai_brake_difference
 *   AI brake difference indicator (normalized).
 */
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

typedef struct packet_s
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
} packet_t;
