#include "global.hpp"
#include "mecanum_drive.hpp"
#include "autons.hpp"  
#include "driver_profile.hpp"

/** Define Variables  */
int current_profile_selection = 0; // Currently selected profile index
std::vector<ProfileOption> profile_list = {
    {"Default", default_profile},
    {"Calvin", calvin_profile},
    {"IDK", unknown_profile}
};

int current_auton_selection = 0;// Currently selected auton index

std::vector<AutonOption> auton_list = {
    {"Left Auton", auton_left},
    {"Right Auton", auton_right},
    {"Skills", auton_skills}
};
int autonColor = 1; // 1 is blue by defualt  -1 is red

/** Define Controllers  */
// Defined VEX PROS Main Master Controller
pros::Controller masterController(pros::E_CONTROLLER_MASTER);

/** Define Motors  */
pros::Motor FUCKTEST(16, pros::MotorGearset::blue);

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

<<<<<<< Updated upstream
=======
pros::MotorGroup motorGroupIntake({15, 17});
>>>>>>> Stashed changes

/** Define Sensors  */
pros::Imu imu(11); 
pros::Rotation horizontal_encoder(5); // horizontal tracking wheel Rotation sensor
pros::Rotation vertical_encoder(6); // vertical tracking wheel Rotation sensor
pros::Optical optical_sensor(7);


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

