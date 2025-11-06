#include "driver_profile.hpp"
#include <vector>  
#include <string> 
#include <cmath>

#include "liblvgl/llemu.hpp"
#include "liblvgl/lvgl.h"
#include "api.h"

#include "global.hpp"

void optSensorTest(){
    pros::Optical optical(13);
    optical.set_led_pwm(100);
    pros::c::optical_rgb_s_t rgb = optical.get_rgb();
    //masterController.set_text(1, 1, std::to_string(rgb.red));
    if(rgb.red > rgb.green && rgb.red > rgb.blue ){
        masterController.set_text(1, 1, "RED");
    }
    else if (rgb.blue > rgb.red && rgb.blue > rgb.green) {
        masterController.set_text(1, 1, "Blue");
    
    }



    //printf("Red Value: %lf \n", rgb.red);
    //printf("Green Value: %lf \n", rgb.green);
    //printf("Blue Value: %lf \n", rgb.blue);
    //printf("Clear value: %lf \n", rgb.brightness);
    
}
void default_profile() {
    optSensorTest();
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