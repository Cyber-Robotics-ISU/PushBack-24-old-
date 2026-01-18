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

bool toggleMegaIntake = false;

bool toggleTopBasket2Top = false;
bool toggleTopBasket2Middle = false;
bool toggleTopBasket2Bottom = false;

bool toggleBottomBasket2Bottom = false;
bool toggleBottomBasket2Middle = false;
bool toggleBottomBasket2Top = false;

bool toggleBottomBasket2TopBasket = false;

void MegaIntakeTask(void*) {
    while (true) {

        if (toggleTopBasket2Top) {
            intakeMotorB.move(-120);
            intakeMotorC.move(120);
            intakeMotorD.move(120);
            intakeMotorE.move(120);
            continue; // Skip everything below and restart the loop
        }

        if (toggleTopBasket2Middle) {
            intakeMotorA.move(120);
            intakeMotorB.move(-120);
            intakeMotorC.move(-120);
            intakeMotorD.move(120);
            continue; // Skip everything below and restart the loop
        }

        if (toggleTopBasket2Bottom) {
            intakeMotorA.move(-120);
            intakeMotorB.move(-120);
            intakeMotorC.move(-120); //
            intakeMotorD.move(120);
            continue; // Skip everything below and restart the loop
        }

        if (toggleBottomBasket2Bottom) {
            intakeMotorA.move(120);
            continue; 
        }

        if (toggleBottomBasket2Middle) {
            intakeMotorA.move(120);
            intakeMotorB.move(120);
            intakeMotorC.move(-120);
            continue; 
        }

        if (toggleBottomBasket2Top) {
            intakeMotorA.move(120);
            intakeMotorB.move(120);
            intakeMotorC.move(120);
            intakeMotorD.move(120);
            intakeMotorE.move(120);
            continue; 
        }

        if (toggleBottomBasket2TopBasket) {
            intakeMotorA.move(120);
            intakeMotorB.move(120);
            intakeMotorC.move(120);
            intakeMotorD.move(120);
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
        intakeMotorA.move(100);
        intakeMotorE.move(-40);

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
                intakeMotorB.move(100);
                intakeMotorD.move(40);
                intakeBEndTime = currentTime + 300;
                intakeDEndTime = currentTime + 300;
            } else if (sendToBottom) {
                intakeMotorB.move(-100);
                intakeMotorD.move(-20);
                intakeBEndTime = currentTime + 350;
                intakeDEndTime = currentTime + 400;
            }
        }

        if (currentTime > intakeBEndTime) intakeMotorB.move(100);
        if (currentTime > intakeDEndTime) intakeMotorD.move(40)  ;

        pros::delay(10);
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

void default_profile_init() {
    const char* colorText = (autonColor == -1) ? "RED" : "BLUE";
    //masterController.set_text(0, 1, std::string("Auton: ") + colorText);
    pros::Task megaIntakeTask(MegaIntakeTask);
    pros::delay(10); 

}

void default_profile_loop() {
    double vx = masterController.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X);
    double vy = -masterController.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
    double omega = masterController.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);

    drive.drive(vx, vy, omega);
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
}

void calvin_profile_init() {
   masterController.set_text(0,1, "test 1");
}

void calvin_profile_loop() {
   
}

void unknown_profile_init() {
    masterController.set_text(0,1, "test 2");
}

void unknown_profile_loop() {
    
}