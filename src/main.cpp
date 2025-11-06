#include "main.h"
#include "global.hpp"
#include "driver_profile.hpp"
#include "mecanum_drive.hpp"

// basic
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdio> 

// lvgl vars
#include "liblvgl/lvgl.h"

#include "ui.hpp" 
#include "pros/apix.h"


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
    pros::lcd::initialize(); // initialize brain screen
    create_main_screen();
}



/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
    auton_list[current_auton_selection].func();
    // Tune PIDs here:
    /**
    drive.setDrivePID(5.0, 0.0, 2.0);   // Forward/backward
    drive.setStrafePID(4.0, 0.0, 2.0);  // Strafing
    drive.setTurnPID(3.0, 0.0, 1.5);    // Turning

    drive.drive_brake_set(pros::E_MOTOR_BRAKE_HOLD); // hold motors when stopped
    drive.drive_imu_calibrate(); // reset IMU heading
    
    drive.moveStraight(24);   // Drive forward 24 inches
    drive.strafePIDMove(12);  // Strafe right 12 inches
    drive.turnPIDAngle(90);       // Turn 90 degrees clockwise
    */
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
	while (true) {	
        profile_list[current_profile_selection].func();
		pros::delay(20); // Run for 20 ms then update
	}
} // End of opcontrol 
