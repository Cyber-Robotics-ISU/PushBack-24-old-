#pragma once
#include "api.h"


// Define global variables and objects here
extern pros::Controller masterController;

// Define VEX Motors

// Define VEX Motor Groups
extern pros::MotorGroup left_motor_group;
extern pros::MotorGroup right_motor_group;

// Define VEX Sensors
extern pros::Imu imu;
extern pros::Rotation horizontal_encoder;
extern pros::Rotation vertical_encoder;