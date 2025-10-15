#pragma once
#include "api.h"
<<<<<<< Updated upstream
=======
#include "mecanum_drive.hpp"
#include "autons.hpp"  
#include "pid.hpp"

extern int current_auton_selection; // Current auton selection
/** Define global variables and objects here */
extern int autonColor;
>>>>>>> Stashed changes

// Auton Option struct
struct AutonOption {
    const char* name;
    void (*func)();
};

// Use a single global variable
extern int current_auton_selection;  // singular
extern std::vector<AutonOption> auton_list;

// Auton Option struct
struct AutonOption {
    const char* name;
    void (*func)();
};

// Use a single global variable
extern int current_auton_selection;  // singular
extern std::vector<AutonOption> auton_list;


// Define global variables and objects here
extern pros::Controller masterController;

<<<<<<< Updated upstream
<<<<<<< Updated upstream
// Define VEX Motors
=======
=======
>>>>>>> Stashed changes
extern pros::Motor FUCKTEST;
/** Define Motors */
extern pros::Motor front_left1;
extern pros::Motor front_left2;
extern pros::Motor front_right1;
extern pros::Motor front_right2;
extern pros::Motor back_left1;
extern pros::Motor back_left2;
extern pros::Motor back_right1;
extern pros::Motor back_right2;
>>>>>>> Stashed changes

// Define VEX Motor Groups
extern pros::MotorGroup left_motor_group;
extern pros::MotorGroup right_motor_group;

// Define VEX Sensors
extern pros::Imu imu;
extern pros::Rotation horizontal_encoder;
extern pros::Rotation vertical_encoder;