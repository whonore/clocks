#ifndef TIMERINTERRUPT_H
#define TIMERINTERRUPT_H

#include <arduino.h>

typedef void (*timer_callback)();

enum {
    HW_TIMER_0 = 0,
    HW_TIMER_1,
    HW_TIMER_2,
    HW_TIMER_3,
    HW_TIMER_4,
    HW_TIMER_5,
    NUM_HW_TIMERS
};

class TimerInterrupt {
  public:
    explicit TimerInterrupt(uint8_t UNUSED(timerNo)) {}

    void init() {}

    bool attachInterruptInterval(unsigned long UNUSED(interval),
                                 timer_callback UNUSED(callback),
                                 unsigned long UNUSED(duration) = 0) {
        return true;
    }
};

TimerInterrupt ITimer1(HW_TIMER_1);

#endif /* TIMERINTERRUPT_H */
