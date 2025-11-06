#include "driver_profile.hpp"
#include <vector>  
#include <string> 
#include <cmath>

#include "liblvgl/lvgl.h"
#include "api.h"

#include "global.hpp"
//toggle 
void default_profile() {
    double vx = masterController.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X);
    double vy = -masterController.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
    double omega = masterController.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);

    drive.drive(vx, vy, omega);

    if (masterController.get_digital(pros::E_CONTROLLER_DIGITAL_R1)) {
        FUCKTEST.move(127);
    }
}

void calvin_profile() {
   
}

void unknown_profile() {
    
}