#pragma once
#include "api.h"
#include <vector>
#include "mecanum_drive.hpp"
#include "autons.hpp"  
#include "driver_profile.hpp"
#include "pid.hpp"

/** Define global variables and objects here */
struct ProfileOption { // profile option struct
    const char* name;
    void (*init)(); // runs once when switching
    void (*loop)(); // runs every cycle
    const char* description; 
};
extern int current_profile_selection; // current profile selection
extern std::vector<ProfileOption> profile_list;

struct AutonOption {
    const char* name;
    const char* description;
    void (*func)();
    int side;  // 0 = red, 1 = blue, 2 = both
};

// -1 = red, 1 = blue
extern int autonColor;
#define IS_BLUE (autonColor == 1)
#define IS_RED  (autonColor == -1)



extern int current_auton_selection;

// master list (ALL autons)
extern std::vector<AutonOption> auton_master_list;

// filtered list (only shows blue/red/both)
extern std::vector<AutonOption> auton_list;

extern pros::adi::Pneumatics scrappePneumatics;
extern pros::adi::Pneumatics stopperPneumatics;

/** Define Controllers  */
// Defined VEX PROS Main Master Controller
extern pros::Controller masterController;
extern pros::Controller slaveController;

extern pros::Motor FUCKTEST;
extern pros::Motor FUCKTEST2;


extern pros::Motor intakeMotorA;
extern pros::Motor intakeMotorB;
extern pros::Motor intakeMotorC;
extern pros::Motor intakeMotorD;
extern pros::Motor intakeMotorE;

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
extern pros::Optical colorCheck;
extern pros::Optical colorCheck2;
extern pros::Rotation horizontal_encoder;
extern pros::Rotation vertical_encoder;

extern MecanumDrive drive;