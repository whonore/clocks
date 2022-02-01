#ifndef ACCELSTEPPER_H
#define ACCELSTEPPER_H

#include <arduino.h>

class AccelStepper {
  public:
    typedef enum {
        FUNCTION = 0,
        DRIVER = 1,
        FULL2WIRE = 2,
        FULL3WIRE = 3,
        FULL4WIRE = 4,
        HALF3WIRE = 6,
        HALF4WIRE = 8
    } MotorInterfaceType;

    AccelStepper(uint8_t UNUSED(interface),
                 uint8_t UNUSED(pin1),
                 uint8_t UNUSED(pin2),
                 uint8_t UNUSED(pin3),
                 uint8_t UNUSED(pin4)) {}

    void setMaxSpeed(float UNUSED(speed)) {}

    void setAcceleration(float UNUSED(acceleration)) {}

    void setCurrentPosition(long UNUSED(position)) {}

    void move(long UNUSED(relative)) {}

    void moveTo(long UNUSED(absolute)) {}

    void runToPosition() {}

    bool run() { return false; }
};

#endif /* ACCELSTEPPER_H */
