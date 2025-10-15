#include "global.hpp"
<<<<<<< Updated upstream

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
=======
#include "mecanum_drive.hpp"
#include "autons.hpp"  

/** Define Variables  */
//Auton Color Side Selected
int current_auton_selection = 0;

std::vector<AutonOption> auton_list = {
    {"Left Auton", auton_left},
    {"Right Auton", auton_right},
    {"Skills", auton_skills}
};
int autonColor = 1; // 1 is blue by defualt  -1 is red

pros::Motor FUCKTEST(16, pros::MotorGearset::blue);

/** Define Controllers  */
// Defined VEX PROS Main Master Controller
pros::Controller masterController(pros::E_CONTROLLER_MASTER);

/** Define Motors  */
// Front-left motors
pros::Motor front_left1(10, pros::MotorGearset::blue);
pros::Motor front_left2(9, pros::MotorGearset::blue);

// Front-right motors (reversed if needed)
pros::Motor front_right1(-20, pros::MotorGearset::blue);
pros::Motor front_right2(-19, pros::MotorGearset::blue);

// Back-left motors
pros::Motor back_left1(1, pros::MotorGearset::blue);
pros::Motor back_left2(2, pros::MotorGearset::blue);

// Back-right motors (reversed if needed)
pros::Motor back_right1(-11, pros::MotorGearset::blue);
pros::Motor back_right2(-12, pros::MotorGearset::blue);

std::vector<pros::Motor*> motorGroupFrontLeft = { &front_left1, &front_left2 };
std::vector<pros::Motor*> motorGroupFrontRight = { &front_right1, &front_right2 };
std::vector<pros::Motor*> motorGroupBackLeft = { &back_left1, &back_left2 };
std::vector<pros::Motor*> motorGroupBackRight = { &back_right1, &back_right2 };


/** Define Sensors  */
pros::Imu imu(21); 
pros::Rotation horizontal_encoder(5); // horizontal tracking wheel Rotation sensor
pros::Rotation vertical_encoder(6); // vertical tracking wheel Rotation sensor
<<<<<<< Updated upstream

MecanumDrive drive(
    {&front_left1, &front_left2},
    {&front_right1, &front_right2},
    {&back_left1, &back_left2},
    {&back_right1, &back_right2},
    &imu,
    &vertical_encoder,
    &horizontal_encoder,
    4.0,   // wheel diameter in inches
    13.5,  // track width
    13.0,  // track base
    3.0    // gear ratio (motor revs per wheel rev)
);

>>>>>>> Stashed changes
=======

MecanumDrive drive(
    {&front_left1, &front_left2},
    {&front_right1, &front_right2},
    {&back_left1, &back_left2},
    {&back_right1, &back_right2},
    &imu,
    &vertical_encoder,
    &horizontal_encoder,
    4.0,   // wheel diameter in inches
    13.5,  // track width
    13.0,  // track base
    3.0    // gear ratio (motor revs per wheel rev)
);

>>>>>>> Stashed changes
