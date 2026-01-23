#include "autons.hpp"
#include "global.hpp"

#include "liblvgl/lvgl.h"
#include "api.h"

#include <vector>  
#include <string> 
#include <cmath>

// Actual autonomous implementations
void auton_left() {
    FUCKTEST.move(20);
}

void auton_right() {

}

void auton_skills() {
    FUCKTEST.move(80);
}
