#include "global.hpp"
#include "mecanum_drive.hpp"
#include "autons.hpp"  
#include "driver_profile.hpp"

/** Define Variables  */
int current_profile_selection = 0; // Currently selected profile index
std::vector<ProfileOption> profile_list = {
    {"Default", default_profile_init, default_profile_loop, "Basic default driving profile"},
    {"Calvin", calvin_profile_init, calvin_profile_loop, "precision driver profile"},
    {"IDK", unknown_profile_init, unknown_profile_loop, "Experimental profile"}
};

// -1 = red, 1 = blue
int autonColor = 1; // default blue

int current_auton_selection = 0;

// ALL autons stored here
std::vector<AutonOption> auton_master_list = {
    { "EXAMPLE",
      "1234567890123456789\n1234567890123456789\n1234567890123456789\n1234567890123456789\n1234567890123456789",
      auton_skills,
      2 }, // blue

    { "RED",
      "Rush the middle mogo. Scores 2 rings.\nFast and consistent.",
      auton_left,
      0 }, // red

    { "BLUE",
      "Blue version of Mogo Rush.\nScores 2 rings.",
      auton_right,
      1 }, // blue

    { "Skills Auton",
      "Runs full skills path.\nWorks on red or blue.",
      auton_skills,
      2 }, // both
};

// Starts empty — will be filled based on autonColor
std::vector<AutonOption> auton_list = {};

/** Define Controllers  */
// Defined VEX PROS Main Master Controller
pros::Controller masterController(pros::E_CONTROLLER_MASTER);

/** Define Motors  */
pros::Motor FUCKTEST(21, pros::MotorGearset::blue);
pros::Motor FUCKTEST2(21, pros::MotorGearset::blue);

pros::Motor intakeMotorA(-1, pros::MotorGearset::blue);
bool intakeToggleA = false;
pros::Motor intakeMotorB(2, pros::MotorGearset::blue);
bool intakeToggleB = false;
pros::Motor intakeMotorC(3, pros::MotorGearset::blue);
bool intakeToggleC = false;
pros::Motor intakeMotorD(4, pros::MotorGearset::green);
bool intakeToggleD = false;
pros::Motor intakeMotorE(16, pros::MotorGearset::green);
bool intakeToggleE = false;

// Front-left motors
pros::Motor front_left1(-17, pros::MotorGearset::blue);
pros::Motor front_left2(-18, pros::MotorGearset::blue);

// Front-right motors (reversed if needed)
pros::Motor front_right1(7, pros::MotorGearset::blue);
pros::Motor front_right2(8, pros::MotorGearset::blue);

// Back-left motors
pros::Motor back_left1(-19, pros::MotorGearset::blue);
pros::Motor back_left2(-20, pros::MotorGearset::blue);

// Back-right motors (reversed if needed)
pros::Motor back_right1(9, pros::MotorGearset::blue);
pros::Motor back_right2(10, pros::MotorGearset::blue);

std::vector<pros::Motor*> motorGroupFrontLeft = { &front_left1, &front_left2 };
std::vector<pros::Motor*> motorGroupFrontRight = { &front_right1, &front_right2 };
std::vector<pros::Motor*> motorGroupBackLeft = { &back_left1, &back_left2 };
std::vector<pros::Motor*> motorGroupBackRight = { &back_right1, &back_right2 };


/** Define Sensors  */
pros::Imu imu(11); 
pros::Optical colorCheck(6);
pros::Optical colorCheck2(11);
pros::Rotation horizontal_encoder(21); // horizontal tracking wheel Rotation sensor
pros::Rotation vertical_encoder(21); // vertical tracking wheel Rotation sensor

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

