#include "global.hpp"


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
      auton_left,
      2 }, // blue

    { "RED",
      "Rush the middle mogo. Scores 2 rings.\nFast and consistent.",
      auton_right,
      0 }, // red

    { "BLUE",
      "Blue version of Mogo Rush.\nScores 2 rings.",
      auton_left,
      1 }, // blue

    { "Skills Auton",
      "Runs full skills path.\nWorks on red or blue.",
      auton_left,
      2 }, // both
};

// Starts empty — will be filled based on autonColor
std::vector<AutonOption> auton_list = {};

/** Define Controllers  */
// Defined VEX PROS Main Master Controller
pros::Controller masterController(pros::E_CONTROLLER_MASTER);
pros::Controller slaveController(pros::E_CONTROLLER_PARTNER);

/** Define Motors  */
pros::Motor FUCKTEST(22, pros::MotorGearset::blue);
pros::Motor FUCKTEST2(22, pros::MotorGearset::blue);

pros::Motor intakeMotorA(-2, pros::MotorGearset::green);
pros::Motor intakeMotorB(3, pros::MotorGearset::blue);
pros::Motor intakeMotorC(9, pros::MotorGearset::blue);
pros::Motor intakeMotorD(-8, pros::MotorGearset::green);
pros::Motor intakeMotorE(7, pros::MotorGearset::green);

// Front-left motors
// NOTE: If motors in a pair fight each other, flip the sign on one motor.
pros::Motor front_left1(14, pros::MotorGearset::blue);
pros::Motor front_left2(12, pros::MotorGearset::blue);

// Front-right motors (reversed if needed)
pros::Motor front_right1(17, pros::MotorGearset::blue);
pros::Motor front_right2(18, pros::MotorGearset::blue);

// Back-left motors
pros::Motor back_left1(13, pros::MotorGearset::blue);
pros::Motor back_left2(11, pros::MotorGearset::blue);

// Back-right motors (reversed if needed)
pros::Motor back_right1(19, pros::MotorGearset::blue);
pros::Motor back_right2(20, pros::MotorGearset::blue);

std::vector<pros::Motor*> motorGroupFrontLeft = { &front_left1, &front_left2 };
std::vector<pros::Motor*> motorGroupFrontRight = { &front_right1, &front_right2 };
std::vector<pros::Motor*> motorGroupBackLeft = { &back_left1, &back_left2 };
std::vector<pros::Motor*> motorGroupBackRight = { &back_right1, &back_right2 };


/** Define Sensors  */
pros::Imu imu(21); 
pros::Optical colorCheck(10);
pros::Optical colorCheck2(1);
pros::Rotation horizontal_encoder(16); // horizontal tracking wheel Rotation sensor
pros::Rotation vertical_encoder(15); // vertical tracking wheel Rotation sensor

MecanumDrive drive(
    motorGroupFrontLeft,
    motorGroupFrontRight,
    motorGroupBackLeft,
    motorGroupBackRight,
    &imu,
    &vertical_encoder,
    &horizontal_encoder,
    2, // Wheel Diameter (Double check this! 2.0 seems small for Mecanum, usually 3.25 or 4)
    13.5, // Track Width
    12.5, // Track Base
    1   // Gear Ratio (1.0 for direct drive blue cart)
);

