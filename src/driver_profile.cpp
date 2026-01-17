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

void toggleIntakeA(int velocity, int direction){
    intakeMotorA.set_reversed(false);
    if(!intakeToggleA){
        intakeMotorA.move(velocity * direction);
        pros::delay(10);
        intakeToggleA = true;
    } else {
        intakeMotorA.move(0);
        pros::delay(10);
        intakeToggleA = false;
    }
}

void toggleIntakeB(int velocity, int direction){
    intakeMotorB.set_reversed(false);
    if(!intakeToggleB){
        intakeMotorB.move(velocity * direction);
        pros::delay(10);
        intakeToggleB = true;
    } else {
        intakeMotorB.move(0);
        pros::delay(10);
        intakeToggleB = false;
    }
}

void toggleIntakeC(int velocity, int direction){
    intakeMotorC.set_reversed(false);
    if(!intakeToggleC){
        intakeMotorC.move(velocity * direction);
        pros::delay(10);
        intakeToggleC = true;
    } else {
        intakeMotorC.move(0);
        pros::delay(10);
        intakeToggleC = false;
    }
}

void toggleIntakeD(int velocity, int direction){
    intakeMotorD.set_reversed(false);
    if(!intakeToggleD){
        intakeMotorD.move(velocity * direction);
        pros::delay(10);
        intakeToggleD = true;
    } else {
        intakeMotorD.move(0);
        pros::delay(10);
        intakeToggleD = false;
    }
}

void toggleIntakeE(int velocity, int direction){
    intakeMotorE.set_reversed(false);
    if(!intakeToggleE){
        intakeMotorE.move(velocity * direction);
        pros::delay(10);
        intakeToggleE = true;
    } else {
        intakeMotorE.move(0);
        pros::delay(10);
        intakeToggleE = false;
    }
}

void test(){
    pros::c::optical_rgb_s_t rgb = colorCheck.get_rgb();
    std::cout << "Red: " << rgb.red << " Green: " << rgb.green 
            << " Blue: " << rgb.blue << " Brightness: " << rgb.brightness << std::endl;
        if (IS_BLUE) {  
            if (rgb.blue > rgb.green && rgb.blue > rgb.red && colorCheck.get_proximity() >= 200) {
                FUCKTEST.move(100);
            pros::delay(1);
            } else {
                FUCKTEST.move(0);
            }


        } else if (IS_RED) {
            if (rgb.red > rgb.green && rgb.red > rgb.blue && colorCheck.get_proximity() >= 200) {
                FUCKTEST.move(-100);
                pros::delay(1);
            } else {
                FUCKTEST.move(0);
            }
        }
}


/** 
void MegaIntake(){
        uint32_t currentTime = pros::millis();
        colorCheck.set_led_pwm(100); 
        int proximity = colorCheck.get_proximity();
        pros::c::optical_rgb_s_t rgb = colorCheck.get_rgb();
        std::cout << "Red: " << rgb.red << " Green: " << rgb.green 
                << " Blue: " << rgb.blue << " Brightness: " << rgb.brightness << std::endl;
        // check auton color first
        toggleIntakeA(120, -1);  
        toggleIntakeD(-60, -1);
            if (IS_BLUE) {  
                // 1. TIMER CHECK: If we are in the middle of a timed move, stay there.
                if (currentTime < intakeBEndTime) {
                    // Do nothing, let the motor finish its 500ms spin
                } 
                // 2. BLUE DETECTED: Score it (Move 120 for 500ms)
                else if (rgb.blue > rgb.red && rgb.blue > rgb.green && proximity >= 100) {
                    intakeBEndTime = currentTime + 1400;
                    intakeMotorB.move(120);
                } 
                // 3. RED DETECTED: Eject it (Move -120 for 500ms)
                else if (rgb.red > rgb.blue && rgb.red > rgb.green && proximity >= 100) {
                    intakeBEndTime = currentTime + 1400;
                    intakeMotorB.move(-120);
                    toggleIntakeD(60, -1);
                } 
                // 4. DEFAULT: No ring seen, and no timer active.
                else {
                    // Keep the intake running forward normally
                    intakeMotorB.move(120);
                }
                toggleIntakeC(-120, -1); 
                toggleIntakeE(120, -1); 
             } else {
                if (currentTime < intakeBEndTime) {
                    } 
                else if (rgb.red > rgb.blue && rgb.red > rgb.green && proximity >= 100) {
                    intakeBEndTime = currentTime + 1400;
                    intakeMotorB.move(120);
                    } 
                else if (rgb.blue > rgb.blue && rgb.blue > rgb.green && proximity >= 100) {
                    intakeBEndTime = currentTime + 1400;
                    intakeMotorB.move(-120);
                    toggleIntakeD(60, -1);
                    } 
                else {
                    intakeMotorB.move(120);
                }
                toggleIntakeC(-120, -1); 
                toggleIntakeE(120, -1); 
        }
}
*/

/** 
void MegaIntake() {
    uint32_t currentTime = pros::millis();
    pros::c::optical_rgb_s_t rgb = colorCheck.get_rgb();
    int proximity = colorCheck.get_proximity();

    // Constant running motors (A, C, E)
    intakeMotorA.move(120); 
    intakeMotorC.move(120); 
    intakeMotorE.move(-40); 

    if (IS_BLUE) {  
        if (currentTime < intakeBEndTime) {
            // Timer active: Let Motor B and D finish their timed movement
        } 
        else if (rgb.blue > rgb.red && rgb.blue > rgb.green && proximity >= 100) {
            // BLUE RING: Score
            intakeBEndTime = currentTime + 1400;
            intakeMotorB.move(120);
            //intakeMotorD.move(60); // Assuming D helps score
        } 
        else if (rgb.red > rgb.blue && rgb.red > rgb.green && proximity >= 100) {
            // RED RING: Eject
            intakeBEndTime = currentTime + 1400;
            intakeMotorB.move(-120);
            intakeMotorD.move(-60); // Reversing D to help eject
        } 
        else {
            // DEFAULT: Move forward
            intakeMotorB.move(120);
            intakeMotorD.move(60); 
        }
    } 
    else if (IS_RED) { // RED ALLIANCE
        if (currentTime < intakeBEndTime) {
            // Timer active
        } 
        else if (rgb.red > rgb.blue && rgb.red > rgb.green && proximity >= 100) {
            // RED RING: Score
            intakeBEndTime = currentTime + 1400;
            intakeMotorB.move(120);
            //intakeMotorD.move(60);
        } 
        else if (rgb.blue > rgb.red && proximity >= 100) {
            // BLUE RING: Eject (Fixed the rgb.blue > rgb.blue typo)
            intakeBEndTime = currentTime + 1400;
            intakeMotorB.move(-120);
            intakeMotorD.move(-60);
        } 
        else {
            intakeMotorB.move(120);
            intakeMotorD.move(60);
        }
    }
}
**/

void MegaIntakeTask(void*) {
    while (true) {

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
        bool sensor2WrongColor =
            (IS_BLUE && redRing2);  // red robot sees blue
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
                intakeMotorD.move(20);
                intakeBEndTime = currentTime + 300;
                intakeDEndTime = currentTime + 300;
            } else if (sendToBottom) {
                intakeMotorB.move(-100);
                intakeMotorD.move(-20);
                intakeBEndTime = currentTime + 300;
                intakeDEndTime = currentTime + 300;
            }
        }

        if (currentTime > intakeBEndTime) intakeMotorB.move(100);
        if (currentTime > intakeDEndTime) intakeMotorD.move(40)  ;

        pros::delay(10);
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

        intakeToggleA = false;
        intakeToggleB = false;
        intakeToggleC = false;
        intakeToggleD = false;
        intakeToggleE = false;
    }


    if (masterController.get_digital(pros::E_CONTROLLER_DIGITAL_R1)) {
        FUCKTEST.move(127);
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