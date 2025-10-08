#include "telemetry.h"

#include <stdio.h>

void print_is_race_on(const is_race_on_t *is_race_on)
{
	is_race_on->flag ? printf("Race is ON\n") : printf("Race is OFF\n");
}

void print_timestamp(const timestamp_t *timestamp)
{
	printf("Timestamp: %u ms\n", timestamp->ms);
}

void print_engine(const engine_t *engine)
{
	printf("Engine:\n");
	printf("\tMax RPM: %.2f\n", engine->max_rpm);
	printf("\tIdle RPM: %.2f\n", engine->idle_rpm);
	printf("\tCurrent RPM: %.2f\n", engine->current_rpm);
}

void print_acceleration(const acceleration_t *acceleration)
{
	printf("Acceleration:\n");
	printf("\tX: %.2f\n", acceleration->x);
	printf("\tY: %.2f\n", acceleration->y);
	printf("\tZ: %.2f\n", acceleration->z);
}
void print_velocity(const velocity_t *velocity)
{
	printf("Velocity:\n");
	printf("\tX: %.2f\n", velocity->x);
	printf("\tY: %.2f\n", velocity->y);
	printf("\tZ: %.2f\n", velocity->z);
}
void print_angular_velocity(const angular_velocity_t *angular_velocity)
{
	printf("Angular Velocity:\n");
	printf("\tX: %.2f\n", angular_velocity->x);
	printf("\tY: %.2f\n", angular_velocity->y);
	printf("\tZ: %.2f\n", angular_velocity->z);
}
void print_rotation(const rotation_t *rotation)
{
	printf("Rotation:\n");
	printf("\tYaw: %.2f\n", rotation->yaw);
	printf("\tPitch: %.2f\n", rotation->pitch);
	printf("\tRoll: %.2f\n", rotation->roll);
}
void print_position(const position_t *position)
{
	printf("Position:\n");
	printf("\tX: %.2f\n", position->x);
	printf("\tY: %.2f\n", position->y);
	printf("\tZ: %.2f\n", position->z);
}

void print_normalized_suspension_travel(const normalized_suspension_travel_t *normalized_suspension_travel)
{
	printf("Normalized Suspension Travel:\n");
	printf("\tFront Left: %.2f\n", normalized_suspension_travel->front_left);
	printf("\tFront Right: %.2f\n", normalized_suspension_travel->front_right);
	printf("\tRear Left: %.2f\n", normalized_suspension_travel->rear_left);
	printf("\tRear Right: %.2f\n", normalized_suspension_travel->rear_right);
}
void print_tire_slip_ratio(const tire_slip_ratio_t *tire_slip_ratio)
{
	printf("Tire Slip Ratio:\n");
	printf("\tFront Left: %.2f\n", tire_slip_ratio->front_left);
	printf("\tFront Right: %.2f\n", tire_slip_ratio->front_right);
	printf("\tRear Left: %.2f\n", tire_slip_ratio->rear_left);
	printf("\tRear Right: %.2f\n", tire_slip_ratio->rear_right);
}
void print_wheel_rotation_speed(const wheel_rotation_speed_t *wheel_rotation_speed)
{
	printf("Wheel Rotation Speed:\n");
	printf("\tFront Left: %.2f\n", wheel_rotation_speed->front_left);
	printf("\tFront Right: %.2f\n", wheel_rotation_speed->front_right);
	printf("\tRear Left: %.2f\n", wheel_rotation_speed->rear_left);
	printf("\tRear Right: %.2f\n", wheel_rotation_speed->rear_right);
}
void print_wheel_on_rumble_strip(const wheel_on_rumble_strip_t *wheel_on_rumble_strip)
{
	printf("Wheel On Rumble Strip:\n");
	printf("\tFront Left: %.2f\n", wheel_on_rumble_strip->front_left);
	printf("\tFront Right: %.2f\n", wheel_on_rumble_strip->front_right);
	printf("\tRear Left: %.2f\n", wheel_on_rumble_strip->rear_left);
	printf("\tRear Right: %.2f\n", wheel_on_rumble_strip->rear_right);
}

void print_wheel_in_puddle_depth(const wheel_in_puddle_depth_t *wheel_in_puddle_depth)
{
	printf("Wheel In Puddle Depth:\n");
	printf("\tFront Left: %.2f\n", wheel_in_puddle_depth->front_left);
	printf("\tFront Right: %.2f\n", wheel_in_puddle_depth->front_right);
	printf("\tRear Left: %.2f\n", wheel_in_puddle_depth->rear_left);
	printf("\tRear Right: %.2f\n", wheel_in_puddle_depth->rear_right);
}
void print_surface_rumble(const surface_rumble_t *surface_rumble)
{
	printf("Surface Rumble:\n");
	printf("\tFront Left: %.2f\n", surface_rumble->front_left);
	printf("\tFront Right: %.2f\n", surface_rumble->front_right);
	printf("\tRear Left: %.2f\n", surface_rumble->rear_left);
	printf("\tRear Right: %.2f\n", surface_rumble->rear_right);
}
void print_tire_slip_angle(const tire_slip_angle_t *tire_slip_angle)
{
	printf("Tire Slip Angle:\n");
	printf("\tFront Left: %.2f\n", tire_slip_angle->front_left);
	printf("\tFront Right: %.2f\n", tire_slip_angle->front_right);
	printf("\tRear Left: %.2f\n", tire_slip_angle->rear_left);
	printf("\tRear Right: %.2f\n", tire_slip_angle->rear_right);
}
void print_tire_combined_slip(const tire_combined_slip_t *tire_combined_slip)
{
	printf("Tire Combined Slip:\n");
	printf("\tFront Left: %.2f\n", tire_combined_slip->front_left);
	printf("\tFront Right: %.2f\n", tire_combined_slip->front_right);
	printf("\tRear Left: %.2f\n", tire_combined_slip->rear_left);
	printf("\tRear Right: %.2f\n", tire_combined_slip->rear_right);
}
void print_suspension_travel_meters(const suspension_travel_meters_t *suspension_travel_meters)
{
	printf("Suspension Travel Meters:\n");
	printf("\tFront Left: %.2f\n", suspension_travel_meters->front_left);
	printf("\tFront Right: %.2f\n", suspension_travel_meters->front_right);
	printf("\tRear Left: %.2f\n", suspension_travel_meters->rear_left);
	printf("\tRear Right: %.2f\n", suspension_travel_meters->rear_right);
}
void print_tire_temp(const tire_temp_t *tire_temp)
{
	printf("Tire Temperature:\n");
	printf("\tFront Left: %.2f\n", tire_temp->front_left);
	printf("\tFront Right: %.2f\n", tire_temp->front_right);
	printf("\tRear Left: %.2f\n", tire_temp->rear_left);
	printf("\tRear Right: %.2f\n", tire_temp->rear_right);
}

void print_car(const car_t *car)
{
	const char *classification_str;
	switch (car->classification)
	{
	case 0:
		classification_str = "D";
		break;
	case 1:
		classification_str = "C";
		break;
	case 2:
		classification_str = "B";
		break;
	case 3:
		classification_str = "A";
		break;
	case 4:
		classification_str = "S1";
		break;
	case 5:
		classification_str = "S2";
		break;
	case 6:
		classification_str = "X";
		break;
	default:
		classification_str = "Unknown";
		break;
	}

	const char *drivetrain_type_str;
	switch (car->drivetrain_type)
	{
	case 0:
		drivetrain_type_str = "FWD";
		break;
	case 1:
		drivetrain_type_str = "RWD";
		break;
	case 2:
		drivetrain_type_str = "AWD";
		break;
	default:
		drivetrain_type_str = "Unknown";
		break;
	}

	printf("Car:\n");
	printf("\tOrdinal: %d\n", car->ordinal);
	printf("\tClassification: %s (%d)\n", classification_str, car->classification);
	printf("\tPerformance Index: %d\n", car->performance_index);
	printf("\tDrivetrain Type: %s (%d)\n", drivetrain_type_str, car->drivetrain_type);
	printf("\tNumber of Cylinders: %d\n", car->num_cylinders);
	printf("\tType: %d\n", car->type);
}
void print_object_hit(const object_hit_t *object_hit)
{
	printf("Object Hit:\n");
	for (int i = 0; i < 6; i++)
	{
		printf("\tObject %d: %c\n", i + 1, object_hit->obj[i]);
	}
}
void print_race(const race_t *race)
{
	printf("Race:\n");
	printf("\tDistance Traveled: %.2f\n", race->distance_traveled);
	printf("\tBest Lap: %.2f\n", race->best_lap);
	printf("\tLast Lap: %.2f\n", race->last_lap);
	printf("\tCurrent Lap: %.2f\n", race->current_lap);
	printf("\tCurrent Time: %.2f\n", race->current_time);
	printf("\tLap Number: %d\n", race->lap_number);
	printf("\tPosition: %d\n", race->position);
}
void print_vehicle_control(const vehicle_control_t *vehicle_control)
{
	printf("Vehicle Control:\n");
	printf("\tAcceleration: %d\n", vehicle_control->accel);
	printf("\tBrake: %d\n", vehicle_control->brake);
	printf("\tClutch: %d\n", vehicle_control->clutch);
	printf("\tHandbrake: %d\n", vehicle_control->handbrake);
	printf("\tGear: %d\n", vehicle_control->gear);
	printf("\tSteering: %d\n", vehicle_control->steer);
}

void print_speed(const speed_t *speed)
{
	printf("Speed:\n");
	printf("\tValue: %.2f\n", speed->value);
}
void print_power(const power_t *power)
{
	printf("Power:\n");
	printf("\tValue: %.2f\n", power->value);
}
void print_torque(const torque_t *torque)
{
	printf("Torque:\n");
	printf("\tValue: %.2f\n", torque->value);
}
void print_boost(const boost_t *boost)
{
	printf("Boost:\n");
	printf("\tValue: %.2f\n", boost->value);
}
void print_fuel(const fuel_t *fuel)
{
	printf("Fuel:\n");
	printf("\tPercent: %.2f\n", fuel->percent);
}
void print_normalized_driving_line(const normalized_driving_line_t *normalized_driving_line)
{
	printf("Normalized Driving Line:\n");
	printf("\tValue: %d\n", normalized_driving_line->value);
}

void print_normalized_ai_brake_difference(const normalized_ai_brake_difference_t *normalized_ai_brake_difference)
{
	printf("Normalized AI Brake Difference:\n");
	printf("\tValue: %d\n", normalized_ai_brake_difference->value);
}

void print_telemetry(const telemetry_t *telemetry)
{
	print_is_race_on(&telemetry->is_race_on);
	print_timestamp(&telemetry->timestamp);
	print_engine(&telemetry->engine);
	print_acceleration(&telemetry->acceleration);
	print_velocity(&telemetry->velocity);
	print_angular_velocity(&telemetry->angular_velocity);
	print_rotation(&telemetry->rotation);
	print_position(&telemetry->position);
	print_normalized_suspension_travel(&telemetry->normalized_suspension_travel);
	print_tire_slip_ratio(&telemetry->tire_slip_ratio);
	print_wheel_rotation_speed(&telemetry->wheel_rotation_speed);
	print_wheel_on_rumble_strip(&telemetry->wheel_on_rumble_strip);
	print_wheel_in_puddle_depth(&telemetry->wheel_in_puddle_depth);
	print_surface_rumble(&telemetry->surface_rumble);
	print_tire_slip_angle(&telemetry->tire_slip_angle);
	print_tire_combined_slip(&telemetry->tire_combined_slip);
	print_suspension_travel_meters(&telemetry->suspension_travel_meters);
	print_tire_temp(&telemetry->tire_temp);
	print_car(&telemetry->car);
	print_object_hit(&telemetry->object_hit);
	print_race(&telemetry->race);
	print_vehicle_control(&telemetry->vehicle_control);
	print_speed(&telemetry->speed);
	print_power(&telemetry->power);
	print_torque(&telemetry->torque);
	print_boost(&telemetry->boost);
	print_fuel(&telemetry->fuel);
	print_normalized_driving_line(&telemetry->normalized_driving_line);
	print_normalized_ai_brake_difference(&telemetry->normalized_ai_brake_difference);
}
