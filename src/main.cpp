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

    imu.reset();
    vertical_encoder.reset();
    vertical_encoder.reset();
    drive.drive_imu_calibrate();

    drive.setFieldOriented(false);
    drive.drive_brake_set(pros::E_MOTOR_BRAKE_HOLD);
    front_right1.set_reversed(false);
    front_right2.set_reversed(false);
    back_right1.set_reversed(false);
    back_right2.set_reversed(false);

    // Left side NOT reversed
    front_left1.set_reversed(true);
    front_left2.set_reversed(true);
    back_left1.set_reversed(true);
    back_left2.set_reversed(true);
    
    drive.setDrivePID(6.25, 0.55, 0.14);   // Forward/backward max speed 125/127
    drive.setStrafePID(4.0, 0.0, 2.0);  // Strafing
    drive.setTurnPID(5, 1.15, 0.5);    // Turning // max speed 100/127
    vertical_encoder.set_reversed(true);

    startMegaIntakeTask();
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
    // Keep auton_list synced even when the UI never opened.
    updateAutonList();

    const std::vector<AutonOption>* list = &auton_list;
    if (list->empty()) {
        list = &auton_master_list; // fallback if filtering produced nothing
    }

    if (list->empty()) {
        auton_left(); // last-resort default
        return;
    }

    int idx = current_auton_selection;
    if (idx < 0 || idx >= (int)list->size()) idx = 0;

    (*list)[idx].func();
    // Tune PIDs here:
    /**
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
    pros::delay(20);

	int last_profile_selection = current_profile_selection;
    profile_list[current_profile_selection].init();
    resetAllFlags();
	while (true) {
         if (current_profile_selection != last_profile_selection) {
            profile_list[current_profile_selection].init();
            last_profile_selection = current_profile_selection;
        }

        profile_list[current_profile_selection].loop();
		pros::delay(20);                               // Run for 20 ms then update
	}
} // End of opcontrol 
