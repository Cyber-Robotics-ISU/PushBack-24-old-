#pragma once

#include <cmath>
#include <vector>
#include <string>

#include "liblvgl/lvgl.h"
#include "api.h"

#include "global.hpp"


// -------- Shared intake task handle --------
extern pros::Task* megaIntakeTaskHandle;

void MegaIntakeTask(void*);
void startMegaIntakeTask();
void stopMegaIntakeTask();

// -------- Shared driver/auton flags --------
extern bool toggleMegaIntake;
extern bool strafeEnabled;

extern bool toggleTopBasket2Top;
extern bool toggleTopBasket2Middle;
extern bool toggleTopBasket2Bottom;
extern bool toggleTopBasket2TopReverse;
extern bool toggleTopBasket2BottomReverse;

extern bool toggleBottomBasket2Bottom;
extern bool toggleBottomBasket2Middle;
extern bool toggleBottomBasket2Top;
extern bool toggleBottomBasket2TopBasket;
extern bool toggleMegaIntakeReverse;

// -------- Intake Scoring Functions --------
void topBasketScoreTop();
void topBasketScoreMiddle();
void topBasketScoreBottom();

void BottomBasketScoreBottom();
void BottomBasketScoreMiddle();
void BottomBasketScoreTop();
void BottomBasket2TopBasket();

void resetAllFlags(); 

// Function declarations of different user profiles for driver control
void default_profile_init();
void simon_profile_init();
void simon_comp_profile_init();
void calvin_profile_init();
void hman_profile_init();
void jack_comp_profile_init();
void unknown_profile_init();

void default_profile_loop();
void simon_profile_loop();
void simon_comp_profile_loop();
void calvin_profile_loop();
void hman_profile_loop();
void jack_comp_profile_loop();
void unknown_profile_loop();
