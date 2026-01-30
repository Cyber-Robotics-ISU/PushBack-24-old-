#include "driver_profile.hpp"
#include <vector>  
#include <string> 
#include <cmath>

#include "liblvgl/lvgl.h"
#include "api.h"

#include "global.hpp"
#include "main.h"

// Stores the time (in milliseconds) when the motor should stop or switch
uint32_t intakeBEndTime = 0;
uint32_t intakeDEndTime = 0; 

pros::Task* megaIntakeTaskHandle = nullptr;

bool strafeEnabled = false;
bool toggleMegaIntake = false;

bool toggleTopBasket2Top = false;
bool toggleTopBasket2Middle = false;
bool toggleTopBasket2Bottom = false;

bool toggleBottomBasket2Bottom = false;
bool toggleBottomBasket2Middle = false;
bool toggleBottomBasket2Top = false;

bool toggleBottomBasket2TopBasket = false;

enum class IntakeMode {
    NONE,
    TOP_BASKET_TOP,
    TOP_BASKET_MIDDLE,
    TOP_BASKET_BOTTOM,
    BUTTOM_BASKET_MIDDLE,
    BUTTOM_BASKET_TOP
};

IntakeMode selectedIntakeMode = IntakeMode::NONE;

void MegaIntakeTask(void*) {
    while (true) {

        if (toggleTopBasket2Top) {
            //intakeMotorA.move(100);
            intakeMotorB.move(-100);
            intakeMotorC.move(125);
            intakeMotorD.move(125);
            intakeMotorE.move(125);
            continue; // Skip everything below and restart the loop
        }

        if (toggleTopBasket2Middle) {
            intakeMotorA.move(125);
            intakeMotorB.move(-125);
            intakeMotorC.move(-125);
            intakeMotorD.move(125);
            continue; // Skip everything below and restart the loop
        }

        if (toggleTopBasket2Bottom) {
            intakeMotorA.move(-125);
            intakeMotorB.move(-50);
            intakeMotorC.move(-50); //
            intakeMotorD.move(125);
            continue; // Skip everything below and restart the loop
        }

        if (toggleBottomBasket2Bottom) {
            intakeMotorA.move(125);
            continue; 
        }

        if (toggleBottomBasket2Middle) {
            intakeMotorA.move(125);
            intakeMotorB.move(125);
            intakeMotorC.move(-125);
            continue; 
        }

        if (toggleBottomBasket2Top) {
            intakeMotorA.move(125);
            intakeMotorB.move(125);
            intakeMotorC.move(125);
            intakeMotorD.move(125);
            intakeMotorE.move(125);
            continue; 
        }

        if (toggleBottomBasket2TopBasket) {
            intakeMotorA.move(125);
            intakeMotorB.move(125);
            intakeMotorC.move(125);
            intakeMotorD.move(125);
            intakeMotorE.move(-30);
            continue; 
        }


        if (!toggleMegaIntake) {
            intakeMotorA.move(0);
            intakeMotorB.move(0);
            intakeMotorC.move(0);
            intakeMotorD.move(0);
            intakeMotorE.move(0);
            pros::delay(10);
            continue;
        }

        uint32_t currentTime = pros::millis();

        // --- Sensor 1 ---
        pros::c::optical_rgb_s_t rgb1 = colorCheck.get_rgb();
        int prox1 = colorCheck.get_proximity();
        colorCheck.set_led_pwm(100);

        // --- Sensor 2 ---
        pros::c::optical_rgb_s_t rgb2 = colorCheck2.get_rgb();
        int prox2 = colorCheck2.get_proximity();
        colorCheck2.set_led_pwm(100);

        // --- Detect rings ---
        bool redRing1  = (prox1 >= 100 && rgb1.red  > rgb1.blue);
        bool blueRing1 = (prox1 >= 100 && rgb1.blue > rgb1.red);

        bool redRing2  = (prox2 >= 100 && rgb2.red  > rgb2.blue);
        bool blueRing2 = (prox2 >= 100 && rgb2.blue > rgb2.red);

        // --- Base intake motors ---
        intakeMotorA.move(125);
        intakeMotorE.move(-120);

        // --- Motor C flips if sensor 2 sees wrong color ---
        int powerC = 120;
        bool sensor2WrongColor = (IS_BLUE && redRing2);  // red robot sees blue
        if (sensor2WrongColor) {
            powerC *= -1; // flip direction
        }
        intakeMotorC.move(powerC);

        // --- Motor B & D routing logic based on sensor 1 only ---
        bool sendToTop = (IS_BLUE && blueRing1) || (IS_RED && redRing1);
        bool sendToBottom = (IS_RED && blueRing1) || (IS_BLUE && redRing1);

        if (currentTime > intakeBEndTime) {
            if (sendToTop) {
                intakeMotorB.move(120);
                intakeMotorD.move(55);
                intakeBEndTime = currentTime + 200;
                intakeDEndTime = currentTime + 200;
            } else if (sendToBottom) {
                intakeMotorB.move(-120);
                intakeMotorD.move(-50);
                intakeBEndTime = currentTime + 250;
                intakeDEndTime = currentTime + 300;
            }
        }

        if (currentTime > intakeBEndTime) intakeMotorB.move(120);
        if (currentTime > intakeDEndTime) intakeMotorD.move(55)  ;

        pros::delay(10);
    }
}

void startMegaIntakeTask() {
    if (megaIntakeTaskHandle == nullptr) {
        megaIntakeTaskHandle = new pros::Task(MegaIntakeTask);
    }
}

void stopMegaIntakeTask() {
    if (megaIntakeTaskHandle != nullptr) {
        megaIntakeTaskHandle->remove();
        delete megaIntakeTaskHandle;
        megaIntakeTaskHandle = nullptr;
    }
}

void topBasketScoreTop() {
    toggleTopBasket2Top = !toggleTopBasket2Top; 
    
    if (toggleTopBasket2Top) {
        // If turning TOP on, turn others OFF
        toggleMegaIntake = false;
        toggleTopBasket2Middle = false;
        toggleTopBasket2Bottom = false;

        toggleBottomBasket2Bottom = false;
        toggleBottomBasket2Middle = false;
        toggleBottomBasket2Top = false;

        toggleBottomBasket2TopBasket = false;
    }
}

void topBasketScoreMiddle() {
    toggleTopBasket2Middle = !toggleTopBasket2Middle; 
    
    if (toggleTopBasket2Middle) {
        // If turning MID on, turn others OFF
        toggleMegaIntake = false;
        toggleTopBasket2Top = false;
        toggleTopBasket2Bottom = false;

        toggleBottomBasket2Bottom = false;
        toggleBottomBasket2Middle = false;
        toggleBottomBasket2Top = false;

        toggleBottomBasket2TopBasket = false;
    }
}

void topBasketScoreBottom() {
    toggleTopBasket2Bottom = !toggleTopBasket2Bottom; 
    
    if (toggleTopBasket2Bottom) {
        // If turning MID on, turn others OFF
        toggleMegaIntake = false;
        toggleTopBasket2Top = false;
        toggleTopBasket2Middle = false;

        toggleBottomBasket2Bottom = false;
        toggleBottomBasket2Middle = false;
        toggleBottomBasket2Top = false;

        toggleBottomBasket2TopBasket = false;
    }
}

void BottomBasketScoreBottom() {
    toggleBottomBasket2Bottom = !toggleBottomBasket2Bottom; 
    
    if (toggleBottomBasket2Bottom) {
        // If turning MID on, turn others OFF
        toggleMegaIntake = false;
        toggleTopBasket2Top = false;
        toggleTopBasket2Middle = false;
        toggleTopBasket2Bottom = false;

        toggleBottomBasket2Middle = false;
        toggleBottomBasket2Top = false;

        toggleBottomBasket2TopBasket = false;
    }
}

void BottomBasketScoreMiddle() {
    toggleBottomBasket2Middle = !toggleBottomBasket2Middle; 
    
    if (toggleBottomBasket2Middle) {
        // If turning MID on, turn others OFF
        toggleMegaIntake = false;
        toggleTopBasket2Top = false;
        toggleTopBasket2Middle = false;
        toggleTopBasket2Bottom = false;

        toggleBottomBasket2Bottom = false;
        toggleBottomBasket2Top = false;

        toggleBottomBasket2TopBasket = false;
    }
}

void BottomBasketScoreTop() {
    toggleBottomBasket2Top = !toggleBottomBasket2Top; 
    
    if (toggleBottomBasket2Top) {
        // If turning MID on, turn others OFF
        toggleMegaIntake = false;
        toggleTopBasket2Top = false;
        toggleTopBasket2Middle = false;
        toggleTopBasket2Bottom = false;

        toggleBottomBasket2Bottom = false;
        toggleBottomBasket2Middle = false;

        toggleBottomBasket2TopBasket = false;
    }
}

void BottomBasket2TopBasket() {
    toggleBottomBasket2TopBasket = !toggleBottomBasket2TopBasket; 
    
    if (toggleBottomBasket2TopBasket) {
        // If turning MID on, turn others OFF
        toggleMegaIntake = false;
        toggleTopBasket2Top = false;
        toggleTopBasket2Middle = false;
        toggleTopBasket2Bottom = false;

        toggleBottomBasket2Bottom = false;
        toggleBottomBasket2Middle = false;
        toggleBottomBasket2Top = false;

    }
}

void resetAllFlags() {
    toggleMegaIntake = false;
    toggleTopBasket2Top = false;
    toggleTopBasket2Middle = false;
    toggleTopBasket2Bottom = false;
    toggleBottomBasket2Bottom = false;
    toggleBottomBasket2Middle = false;
    toggleBottomBasket2Top = false;
    toggleBottomBasket2TopBasket = false;
    
    // Also stop the motors immediately
    intakeMotorA.move(0);
    intakeMotorB.move(0);
    intakeMotorC.move(0);
    intakeMotorD.move(0);
    intakeMotorE.move(0);
}

void partnerIntakeSelector() {
    if (partnerController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_A)) {
        selectedIntakeMode = IntakeMode::TOP_BASKET_TOP;
    }
    else if (partnerController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_B)) {
        selectedIntakeMode = IntakeMode::TOP_BASKET_MIDDLE;
    }
    else if (partnerController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_Y)) {
        selectedIntakeMode = IntakeMode::TOP_BASKET_BOTTOM;
    }
    else if (partnerController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_X)) {
        selectedIntakeMode = IntakeMode::BUTTOM_BASKET_MIDDLE;
    }
    else if (partnerController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_X)) {
        selectedIntakeMode = IntakeMode::BUTTOM_BASKET_TOP;
    }
    pros::delay(10);
}


void runSelectedIntake() {
    switch (selectedIntakeMode) {
        case IntakeMode::TOP_BASKET_TOP:
            topBasketScoreTop();
            break;

        case IntakeMode::TOP_BASKET_MIDDLE:
            topBasketScoreMiddle();
            break;

        case IntakeMode::TOP_BASKET_BOTTOM:
            topBasketScoreBottom();
            break;

        case IntakeMode::BUTTOM_BASKET_MIDDLE:
            BottomBasketScoreMiddle();
            break;

        case IntakeMode::BUTTOM_BASKET_TOP:
            BottomBasketScoreTop();
            break;

        default:
            break;
    }
}

void default_profile_init() {
}

void default_profile_loop() {
    double strafe = strafeEnabled
    ? -masterController.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X)
    : 0.0;
    //double strafe = -masterController.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X); // Left/Right
    double straight = masterController.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y); // Forward/Back
    double turn = masterController.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X); // Turn

    auto cubic = [](double v) {
        return (v * v * v) / (127.0 * 127.0);
    };
    drive.drive((strafe), cubic(straight), cubic(turn)); 

    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_UP)) {
        strafeEnabled = !strafeEnabled;
    }


    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_A)) {
        scrappePneumatics.toggle();
    }

    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_X)) {
        toggleMegaIntake = !toggleMegaIntake;

        if (toggleMegaIntake) {
            toggleTopBasket2Top = false;
            toggleTopBasket2Middle = false;
            toggleTopBasket2Bottom = false;

            toggleBottomBasket2Bottom = false;
            toggleBottomBasket2Middle = false;
            toggleBottomBasket2Top = false;

            toggleBottomBasket2TopBasket = false;
        }
    }

    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1)) {
        topBasketScoreTop(); 
    }
    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
        topBasketScoreMiddle(); 
    }
    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L2)) {
        topBasketScoreBottom(); 
    }

    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)) {
        stopperPneumatics.toggle(); 
    }

    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_B)) {
        BottomBasketScoreTop(); 
    }

    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_DOWN)) {
        BottomBasketScoreMiddle(); 
    }

    pros::delay(10);
}

void simon_profile_init() {
}

void simon_profile_loop() {
    double strafe = strafeEnabled
    ? -masterController.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X)
    : 0.0;
    //double strafe = -masterController.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X); // Left/Right
    double straight = masterController.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y); // Forward/Back
    double turn = masterController.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X); // Turn

    auto cubic = [](double v) {
        return (v * v * v) / (127.0 * 127.0);
    };
    drive.drive((strafe), cubic(straight), cubic(turn)); 

    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_UP)) {
        strafeEnabled = !strafeEnabled;
    }

    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_A)) {
        scrappePneumatics.toggle();
    }

    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1)) {
        toggleMegaIntake = !toggleMegaIntake;

        if (toggleMegaIntake) {
            toggleTopBasket2Top = false;
            toggleTopBasket2Middle = false;
            toggleTopBasket2Bottom = false;

            toggleBottomBasket2Bottom = false;
            toggleBottomBasket2Middle = false;
            toggleBottomBasket2Top = false;

            toggleBottomBasket2TopBasket = false;
        }
    }

    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)) {
        topBasketScoreTop(); 
    }
    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L2)) {
        topBasketScoreMiddle(); 
    }
    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)) {
        topBasketScoreBottom(); 
    }

    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)) {
        stopperPneumatics.toggle(); 
    }

    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_B)) {
        BottomBasketScoreTop(); 
    }

    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_DOWN)) {
        BottomBasketScoreMiddle(); 
    }

    pros::delay(10);
}

void calvin_profile_init() {
   masterController.set_text(0,1, "test 1");
}

void calvin_profile_loop() {
   
}

void hman_profile_init() {
    const char* colorText = (autonColor == -1) ? "RED" : "BLUE";
}

void hman_profile_loop() {
    double strafe = -masterController.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X); // Left/Right
    double straight = masterController.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y); // Forward/Back
    double turn = masterController.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X); // Turn

    auto cubic = [](double v) {
        return (v * v * v) / (127.0 * 127.0);
    };
    drive.drive((strafe), cubic(straight), cubic(turn));

    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
        toggleMegaIntake = !toggleMegaIntake;

        if (toggleMegaIntake) {
            toggleTopBasket2Top = false;
            toggleTopBasket2Middle = false;
            toggleTopBasket2Bottom = false;

            toggleBottomBasket2Bottom = false;
            toggleBottomBasket2Middle = false;
            toggleBottomBasket2Top = false;

            toggleBottomBasket2TopBasket = false;
        }
    }

    partnerIntakeSelector();

    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_LEFT)) {
        runSelectedIntake();
    }
    colorCheck3.set_led_pwm(100);
    double hue = colorCheck3.get_hue();
    if (hue > 15 && hue < 35) {
        masterController.set_text(0,1, "orangey");
        pros::delay(10);
    } else {
         masterController.clear_line(0);
        pros::delay(10);
    }

    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L2)) {
        topBasketScoreTop(); 
    }
    /**
    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1)) {
        topBasketScoreTop(); 
    }
    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
        topBasketScoreMiddle(); 
    }
    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L2)) {
        topBasketScoreBottom(); 
    }

    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)) {
        BottomBasketScoreBottom(); 
    }

    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_UP)) {
        BottomBasketScoreMiddle(); 
    }

    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_DOWN)) {
        BottomBasketScoreTop(); 
    }

    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_LEFT)) {
        BottomBasket2TopBasket(); 
    }
    */
    //
    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)) {
        stopperPneumatics.toggle(); 
    }

    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1)) {
        scrappePneumatics.toggle();
    }
    
    pros::delay(10);
}

void unknown_profile_init() {
    masterController.set_text(0,1, "test 2");
}

void unknown_profile_loop() {
}