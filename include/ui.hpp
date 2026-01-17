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


// --- NEW Required Prototypes ---
void create_auton_color_screen();  // <--- THIS ONE WAS MISSING
void auton_red_select();
void auton_blue_select();

// (If updateAutonList is intended to be used elsewhere)
void updateAutonList();