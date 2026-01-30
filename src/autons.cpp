#include "autons.hpp"
#include "global.hpp"

#include "liblvgl/lvgl.h"
#include "api.h"

#include <vector>  
#include <string> 
#include <cmath>

// Actual autonomous implementations
void auton_left() {
    drive.turnPIDAngle(90, 100);
}

void auton_right() {

}

void auton_skills() {
    FUCKTEST.move(80);
}

void red_auton() {
    toggleMegaIntake = true;
    drive.moveStraight(-25.0);
    topBasketScoreTop();
    pros::delay(1000);

    drive.turnPIDAngle(-90.0);
    drive.moveStraight(25.0);

    resetAllFlags();
}
