#pragma once
#include "api.h"
#include "mecanum_drive.hpp"
#include "autons.hpp"  
#include "pid.hpp"

extern int current_auton_selection; // Current auton selection
/** Define global variables and objects here */
extern int autonColor;

// Auton Option struct
struct AutonOption {
    const char* name;
    void (*func)();
};

// Use a single global variable
extern int current_auton_selection;  // singular
extern std::vector<AutonOption> auton_list;

/** Define Controllers  */
// Defined VEX PROS Main Master Controller
extern pros::Controller masterController;

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

// Motor group vectors (extern)
extern std::vector<pros::Motor*> motorGroupFrontLeft;
extern std::vector<pros::Motor*> motorGroupFrontRight;
extern std::vector<pros::Motor*> motorGroupBackLeft;
extern std::vector<pros::Motor*> motorGroupBackRight;

/** Define Motor Groups */


/** Define Sensors */
extern pros::Imu imu;
extern pros::Rotation horizontal_encoder;
extern pros::Rotation vertical_encoder;

extern MecanumDrive drive;