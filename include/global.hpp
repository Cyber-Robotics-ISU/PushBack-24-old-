#pragma once
#include "api.h"
#include "mecanum_drive.hpp"
#include "autons.hpp"  
#include "driver_profile.hpp"
#include "pid.hpp"

/** Define global variables and objects here */
struct ProfileOption { // profile option struct
    const char* name;
    void (*func)();
};
extern int current_profile_selection; // current profile selection
extern std::vector<ProfileOption> profile_list;

struct AutonOption { // Auton Option struct
    const char* name;
    void (*func)();
};
extern int autonColor;
extern int current_auton_selection; // Current auton selection
extern std::vector<AutonOption> auton_list;



/** Define Controllers  */
// Defined VEX PROS Main Master Controller
extern pros::Controller masterController;

extern pros::Motor FUCKTEST;
/** Define Drive Train Motors */
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