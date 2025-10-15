#pragma once

#include "autons.hpp"  

#include "liblvgl/lvgl.h"
#include <vector>  
#include <string> 
#include <cmath>

// LVGL Screens
void create_main_screen();
void create_auton_screen();
void create_motor_temp_screen();
void create_odometry_screen();
void create_pid_screen();

// Utilities
void clear_screen(lv_obj_t* screen);