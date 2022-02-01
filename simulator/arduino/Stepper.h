#ifndef STEPPER_H
#define STEPPER_H

#include <arduino.h>

class Stepper {
  public:
    Stepper(int UNUSED(number_of_steps),
            int UNUSED(motor_pin_1),
            int UNUSED(motor_pin_2),
            int UNUSED(motor_pin_3),
            int UNUSED(motor_pin4)) {}

    void setSpeed(long UNUSED(whatSpeed)) {}

    void step(int UNUSED(number_of_steps)) {}
};

#endif /* STEPPER_H */
