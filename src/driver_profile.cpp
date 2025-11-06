#include "driver_profile.hpp"
#include <vector>  
#include <string> 
#include <cmath>

#include "liblvgl/lvgl.h"
#include "api.h"

#include "global.hpp"
<<<<<<< Updated upstream
//toggle 
=======

bool intakeToggle = 1;
void toggleIntake(){
    if(!intakeToggle){
        motorGroupIntake.move(127);
        pros::delay(10);
        intakeToggle = true;
    }
    else if(intakeToggle){
        motorGroupIntake.move(0);
        pros::delay(10);
        intakeToggle = false;
    }
}

void testOptical(){
  pros::c::optical_rgb_s_t rgb_value;
    rgb_value = optical_sensor.get_rgb();
    if (rgb_value.blue > rgb_value.green && rgb_value.blue > rgb_value.red){
        masterController.set_text(0, 1, "blue");
    } else if(rgb_value.green > rgb_value.blue && rgb_value.green > rgb_value.red){
        masterController.set_text(1, 1, "red");
    } else if (rgb_value.red > rgb_value.blue && rgb_value.red > rgb_value.green){
        masterController.set_text(2, 1, "green");
    }
    pros::delay(20);
  
}
>>>>>>> Stashed changes
void default_profile() {
    double vx = masterController.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X);
    double vy = -masterController.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
    double omega = masterController.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);

    drive.drive(vx, vy, omega);

    if (masterController.get_digital(pros::E_CONTROLLER_DIGITAL_R1)) {
        FUCKTEST.move(127);
    }
<<<<<<< Updated upstream
=======

    if (masterController.get_digital(pros::E_CONTROLLER_DIGITAL_R1)){
        toggleIntake();
    }
    
    testOptical();
>>>>>>> Stashed changes
}

void calvin_profile() {
   
}

void unknown_profile() {
    
}
