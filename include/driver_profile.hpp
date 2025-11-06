#pragma once

#include <cmath>
#include <vector>
#include <string>

#include "liblvgl/lvgl.h"
#include "api.h"

#include "global.hpp"


// Function declarations of different user profiles for driver control
void default_profile_init();
void calvin_profile_init();
void unknown_profile_init();

void default_profile_loop();
void calvin_profile_loop();
void unknown_profile_loop();