#pragma once
#include <vector>  
#include <string> 
#include <cmath>

#include "autons.hpp"  

#include "liblvgl/lvgl.h"

// LVGL Screens
void create_main_screen();
void create_auton_screen();
void create_profiles_screen();
void create_odometry_screen();
void create_pid_screen();

// Utilities
void clear_screen(lv_obj_t* screen);