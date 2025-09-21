#include "global.hpp"

// Define variables 

// Defined VEX Main Master Controller
pros::Controller masterController(pros::E_CONTROLLER_MASTER);

// Define VEX Motors

// Define VEX Motor Groups
pros::MotorGroup left_motor_group({-1, 2, -3}, pros::MotorGearset::blue); // left motors use 600 RPM cartrifges
pros::MotorGroup right_motor_group({4, -5, 6}, pros::MotorGearset::blue); // right motors use 600 RPM cartridges

// Define VEX Sensors
pros::Imu imu(10); 
pros::Rotation horizontal_encoder(20); // horizontal tracking wheel Rotation sensor
pros::Rotation vertical_encoder(21); // vertical tracking wheel Rotation sensor