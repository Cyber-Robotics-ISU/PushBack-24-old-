---
layout: default
title: Driver Examples
description: Toggle, Hold, and Otjers
---

TBD  

## Initial Setup

### Toggleable Example Showcase
For This example will showcase a toogleable intake
#### Define the Vairable
```cpp
bool intakeToggle = false;
int flipMotors = 1;

// defined pros vex motor
pros::Motor intakeMotor(14, pros::MotorGearset::green);
```


#### Outside Main Function but in the same file.
This function take the input of the velocity -127 to 127 and direction -1 or 1.
```cpp
void toggleIntake(int velocity, int direction){
    intakeMotor.set_reversed(false);
    direction = flipMotors * direction;
    if(!intakeToggle){
        intakeMotor.move(velocity * direction); 
        pros::delay(10);
        intakeToggle = true;
    } else {
        intakeMotor.move(0);
        pros::delay(10);
        intakeToggle = false;
    }
}
```

#### Main Function
When the controller is pressed intake spins at 120 velocity and -1 direction 
``` cpp 
void opcontrol() {
  while(true){
     if (main_Controller.get_digital(DIGITAL_L2)){ 
        toggleIntake(120, -1); // reverse
    } else if (main_Controller.get_digital_new_press(DIGITAL_L1)){
        toggleIntake(120, 1);  // forwards
    }
    // this line of code allows the driver to swtich the overall direction of all motors. 
    if (main_Controller.get_digital_new_press(DIGITAL_RIGHT)){ 
      flipMotors = flipMotors * -1; 
    }
  }
}
```

