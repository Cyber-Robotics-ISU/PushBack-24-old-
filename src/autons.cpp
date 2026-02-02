#include "autons.hpp"
#include "global.hpp"

#include "liblvgl/lvgl.h"
#include "api.h"

#include <vector>  
#include <string> 
#include <cmath>

void test_turn_auton() {
 
}

// Actual autonomous implementations
void auton_left() {
    drive.strafePIDMove(12, 120);
}

void auton_right() {
    drive.moveStraight(12.0);
}

void auton_skills() {
     topBasketScoreTop();
     pros::delay(2000);
     topBasketScoreTop();
     pros::delay(2000);
     topBasketScoreTop();
}

void red_auton_right() {
    drive.resetOdometry();
    startMegaIntakeTask();
    drive.moveStraight(20, 120, 1500);
    //drive.turnPIDAngle(90.5, 120, 1500);
    drive.turnToHeading(90, 120, 1500);
    //drive.moveStraight(2.0, 127, 2000);
    scrappePneumatics.toggle();
    toggleMegaIntake = true;
    pros::delay(10);
    drive.moveStraight(14.0, 127, 2000);
    drive.moveStraight(6.0, 127, 2000);
    drive.moveStraight(-9.0, 127, 2000);
    drive.moveStraight(16.0, 127, 2000);
    pros::delay(100);
    drive.moveStraight(-12.0, 127, 2000);
    scrappePneumatics.toggle();
    //drive.turnPIDAngle(-183.0, 120, 1500);
     drive.turnToHeading(272, 120, 1500);
    drive.moveStraight(8.0, 127, 2000);
    toggleMegaIntake = false;
    drive.strafePIDMove(-4.50, 70, 2000);
    drive.moveStraight(8.75, 127, 2000);
    topBasketScoreTop();
    pros::delay(2000);
    drive.moveStraight(-4, 127, 2000);
    resetAllFlags();
}
