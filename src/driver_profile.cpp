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
const char* colorText = "None";

bool strafeEnabled = true;
bool toggleMegaIntake = false;

bool toggleTopBasket2Top = false;
bool toggleTopBasket2Middle = false;
bool toggleTopBasket2Bottom = false;
bool toggleTopBasket2TopReverse = false;
bool toggleTopBasket2BottomReverse = false;

bool toggleBottomBasket2Bottom = false;
bool toggleBottomBasket2Middle = false;
bool toggleBottomBasket2Top = false;

bool toggleBottomBasket2TopBasket = false;
bool toggleMegaIntakeReverse = false;

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
    static uint32_t lastProxPrint = 0;
    while (true) {
        // Show current auton color on the controller for quick sanity check.
        if (IS_RED) {
            masterController.clear();
            pros::delay(30);
            masterController.set_text(0, 1, "auton: RED");
        } else if (IS_BLUE) {
            masterController.clear();
            pros::delay(50);
            masterController.set_text(0, 1, "auton: BLUE");
        }

        if (toggleTopBasket2TopReverse) {
            intakeMotorB.move(100);
            intakeMotorC.move(-125);
            intakeMotorD.move(-125);
            intakeMotorE.move(-125);
            pros::delay(10);
            continue; // Skip everything below and restart the loop
        }

        if (toggleTopBasket2Top) {
            //intakeMotorA.move(100);
            intakeMotorB.move(-100);
            intakeMotorC.move(125);
            intakeMotorD.move(125);
            intakeMotorE.move(125);
            pros::delay(10);
            continue; // Skip everything below and restart the loop
        }

        if (toggleTopBasket2Middle) {
            intakeMotorA.move(125);
            intakeMotorB.move(-125);
            intakeMotorC.move(-125);
            intakeMotorD.move(125);
            pros::delay(10);
            continue; // Skip everything below and restart the loop
        }

        if (toggleTopBasket2BottomReverse) {
            intakeMotorA.move(125);
            intakeMotorB.move(50);
            intakeMotorC.move(50); //
            intakeMotorD.move(-125);
            pros::delay(10);
            continue; // Skip everything below and restart the loop
        }

        if (toggleTopBasket2Bottom) {
            intakeMotorA.move(-125);
            intakeMotorB.move(-50);
            intakeMotorC.move(-50); //
            intakeMotorD.move(125);
            pros::delay(10);
            continue; // Skip everything below and restart the loop
        }

        if (toggleBottomBasket2Bottom) {
            intakeMotorA.move(125);
            pros::delay(10);
            continue; 
        }

        if (toggleBottomBasket2Middle) {
            intakeMotorA.move(125);
            intakeMotorB.move(125);
            intakeMotorC.move(-125);
            pros::delay(10);
            continue; 
        }

        if (toggleBottomBasket2Top) {
            intakeMotorA.move(125);
            intakeMotorB.move(125);
            intakeMotorC.move(125);
            intakeMotorD.move(125);
            intakeMotorE.move(125);
            pros::delay(10);
            continue; 
        }

        if (toggleBottomBasket2TopBasket) {
            intakeMotorA.move(125);
            intakeMotorB.move(125);
            intakeMotorC.move(125);
            intakeMotorD.move(125);
            intakeMotorE.move(-30);
            pros::delay(10);
            continue; 
        }

        if (toggleMegaIntakeReverse) {
            intakeMotorA.move(-125);
            intakeMotorB.move(-120);
            intakeMotorC.move(-120);
            intakeMotorD.move(-55);
            intakeMotorE.move(120);
            pros::delay(10);
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
        colorCheck.set_led_pwm(100);
        colorCheck2.set_led_pwm(100);
        colorCheck0.set_led_pwm(100);

        colorCheck0.set_integration_time(3); 
        colorCheck.set_integration_time(3);
        colorCheck2.set_integration_time(3);

        // --- Sensor Data Collection (Using Hue) ---
        double hue1 = colorCheck.get_hue();
        int prox1 = colorCheck.get_proximity();

        double hue0 = colorCheck0.get_hue();
        int prox0 = colorCheck0.get_proximity();

        double hue2 = colorCheck2.get_hue();
        int prox2 = colorCheck2.get_proximity();

        // bool redRing1  = (prox1 >= 80 && ((hue1 >= 0 && hue1 <= 55) || (hue0 >= 0 && hue0 <= 55)));
        //bool blueRing1 = (prox1 >= 80 && ((hue1 >= 180 && hue1 <= 255)));
    
        // --- Color Detection Logic ---
        // Blue: 200-225 | Red: 3-15
        //bool redRing1  = (((hue1 >= 0 && hue1 <= 55) || (hue0 >= 0 && hue0 <= 55)));
        //bool blueRing1 = (prox1 >= 80 && ((hue1 >= 180 && hue1 <= 270)));
        const int proxMin = 150;
        bool use0 = prox0 >= prox1;
        double hueBest = use0 ? hue0 : hue1;
        int proxBest = use0 ? prox0 : prox1;

        //bool redRing1  = (proxBest >= proxMin && (hueBest >= 0 && hueBest <= 55));
        //bool blueRing1 = (proxBest >= proxMin && (hueBest >= 180 && hueBest <= 270));

        bool redRing1  = (prox0 >= 100 && hue0 >= 0 && hue0 <= 55);
        bool blueRing1 = (prox0 >= 100 && hue0 >= 190 && hue0 <= 270);

        bool redRing2  = (prox2 >= 80 && hue2 >= 0 && hue2 <= 55);
        bool blueRing2 = (prox2 >= 80 && hue2 >= 200 && hue2 <= 270);


        // --- Base intake motors ---
        intakeMotorA.move(100);
        intakeMotorE.move(-120);

        // --- Motor C: Reject if Sensor 2 sees the WRONG alliance color ---
        int powerC = 115;
        bool isWrongColorS2 = (IS_BLUE && redRing2) || (IS_RED && blueRing2);
        
        if (isWrongColorS2) {
            //powerC = -115; // Reverse to eject the "bad" ring
            // NEW: If you want Motor C to reverse for a set time, 
            // you would set an intakeCEndTime variable here similarly to B and D.
        }
        intakeMotorC.move(powerC);

        // --- Motor B & D Routing: Send to Top vs Reject to Bottom ---
        // "sendToTop" only if it matches our alliance
        bool sendToTop = (IS_BLUE && blueRing1) || (IS_RED && redRing1);
        
        // "sendToBottom" (Reject) if it's the opponent's color
        bool sendToBottom = (IS_BLUE && redRing1) || (IS_RED && blueRing1);
    
        // --- FIXED TRIGGER LOGIC ---
        // If we see a ring we want, we RESET the end time to 'now + duration'
        if (sendToTop) {
            intakeBEndTime = currentTime + 350; // Spins for 350ms after detection
            intakeDEndTime = currentTime + 400; // Spins for 400ms after detection
        }

        // Apply power if we are still within the "set time" window
        if (currentTime < intakeBEndTime) {
            intakeMotorB.move(110);
        } else {
            intakeMotorB.move(110); // Default speed (you can change this to 0 if needed)
        }

        if (currentTime < intakeDEndTime) {
            intakeMotorD.move(50);
        } else {
            intakeMotorD.move(55); // Default speed
        }

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
    toggleTopBasket2TopReverse = false;
    toggleTopBasket2BottomReverse = false;
    toggleBottomBasket2Bottom = false;
    toggleBottomBasket2Middle = false;
    toggleBottomBasket2Top = false;
    toggleBottomBasket2TopBasket = false;
    toggleMegaIntakeReverse = false;
    
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
    else if (partnerController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_RIGHT)) {
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
    drive.drive(cubic(strafe), cubic(straight), cubic(turn)); 

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
    drive.drive(cubic(strafe), cubic(straight), cubic(turn)); 

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
    } else if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L2)) {
        topBasketScoreMiddle(); 
    } else if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
        topBasketScoreBottom(); 
    } else if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_X)) {
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

void simon_comp_profile_init() {
    resetAllFlags();
}

void simon_comp_profile_loop() {
    double strafe = strafeEnabled
    ? -masterController.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X)
    : 0.0;
    double straight = masterController.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y); // Forward/Back
    double turn = masterController.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X); // Turn

    auto cubic = [](double v) {
        return (v * v * v) / (127.0 * 127.0);
    };
    drive.drive(cubic(strafe), cubic(straight), cubic(turn)); 

    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_UP)) {
        strafeEnabled = !strafeEnabled;
    }

    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)) {
        scrappePneumatics.toggle();
    }

    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_A)) {
        stopperPneumatics.toggle(); 
    }

    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_Y)) {
        auton_left(); 
    }

    bool shift = masterController.get_digital(pros::E_CONTROLLER_DIGITAL_L2);
    bool r1 = masterController.get_digital(pros::E_CONTROLLER_DIGITAL_R1);
    bool r2 = masterController.get_digital(pros::E_CONTROLLER_DIGITAL_R2);
    bool down = masterController.get_digital(pros::E_CONTROLLER_DIGITAL_DOWN);
    bool b = masterController.get_digital(pros::E_CONTROLLER_DIGITAL_B);

    toggleMegaIntake = false;
    toggleMegaIntakeReverse = false;
    toggleTopBasket2Top = false;
    toggleTopBasket2TopReverse = false;
    toggleTopBasket2Bottom = false;
    toggleTopBasket2BottomReverse = false;
    toggleTopBasket2Middle = false;
    toggleBottomBasket2Bottom = false;
    toggleBottomBasket2Middle = false;
    toggleBottomBasket2Top = false;
    toggleBottomBasket2TopBasket = false;

    if (shift && r1) {
        toggleMegaIntakeReverse = true;
    } else if (r1) {
        toggleMegaIntake = true;
    } else if (shift && r2) {
        toggleTopBasket2TopReverse = true;
    } else if (r2) {
        toggleTopBasket2Top = true;
    } else if (shift && down) {
        toggleTopBasket2BottomReverse = true;
    } else if (down) {
        toggleTopBasket2Bottom = true;
    } else if (b) {
        toggleTopBasket2Middle = true;
    }

    pros::delay(10);
}

void jack_comp_profile_init() {
    resetAllFlags();
}

void jack_comp_profile_loop() {
    double strafe = strafeEnabled
    ? -masterController.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X)
    : 0.0;
    double straight = masterController.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y); // Forward/Back
    double turn = masterController.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X); // Turn

    auto cubic = [](double v) {
        return (v * v * v) / (127.0 * 127.0);
    };
    drive.drive(cubic(strafe), cubic(straight), cubic(turn)); 

    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_UP)) {
        strafeEnabled = !strafeEnabled;
    }

    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)) {
        scrappePneumatics.toggle();
    }

    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_X)) {
        stopperPneumatics.toggle(); 
    }

    if (masterController.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_Y)) {
        auton_left(); 
    }

    bool shift = masterController.get_digital(pros::E_CONTROLLER_DIGITAL_L2);
    bool r1 = masterController.get_digital(pros::E_CONTROLLER_DIGITAL_R1);
    bool r2 = masterController.get_digital(pros::E_CONTROLLER_DIGITAL_R2);
    bool down = masterController.get_digital(pros::E_CONTROLLER_DIGITAL_DOWN);
    bool b = masterController.get_digital(pros::E_CONTROLLER_DIGITAL_B);
    bool a = masterController.get_digital(pros::E_CONTROLLER_DIGITAL_A);

    toggleMegaIntake = false;
    toggleMegaIntakeReverse = false;
    toggleTopBasket2Top = false;
    toggleTopBasket2TopReverse = false;
    toggleTopBasket2Bottom = false;
    toggleTopBasket2BottomReverse = false;
    toggleTopBasket2Middle = false;
    toggleBottomBasket2Bottom = false;
    toggleBottomBasket2Middle = false;
    toggleBottomBasket2Top = false;
    toggleBottomBasket2TopBasket = false;

    if (shift && r2) {
        toggleMegaIntakeReverse = true;
    } else if (r2) {
        toggleMegaIntake = true;
    } else if (shift && r1) {
        toggleTopBasket2TopReverse = true;
    } else if (r1) {
        toggleTopBasket2Top = true;
    } else if (shift && down) {
        toggleTopBasket2BottomReverse = true;
    } else if (down) {
        toggleTopBasket2Bottom = true;
    } else if (b) {
        toggleTopBasket2Middle = true;
    } else if (a) {
        toggleBottomBasket2TopBasket = true;
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
    drive.drive(cubic(strafe), cubic(straight), cubic(turn));

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
