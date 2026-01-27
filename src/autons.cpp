#include "autons.hpp"
#include "global.hpp"

#include "liblvgl/lvgl.h"
#include "api.h"

#include <vector>  
#include <string> 
#include <cmath>

// Actual autonomous implementations
void auton_left() {
    drive.moveStraight(12, 125);
}

void auton_right() {

}

void auton_skills() {
    FUCKTEST.move(80);
}
