#pragma once

#include <cmath>
#include <vector>
#include <string>

#include "liblvgl/lvgl.h"
#include "api.h"

#include "global.hpp"


// Function declarations of different user profiles for driver control
void default_profile();
void calvin_profile();
void unknown_profile();