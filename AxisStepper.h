#pragma once

#include <Arduino.h>
#include "Models/Directable.h"

#if defined(ARDUINO_GIGA)
#include "Portenta_H7_TimerInterrupt.h"
#elif defined(ARDUINO_SAM_DUE)
#include <DueTimer.h>
#endif

class AxisStepper : public Directable
{
public:
    byte id;
    typedef void (*VoidCallback)();
    bool calibrated = false;
    uint32_t nextStepAt;

    struct XYStepper
    {
        int step_pin;
        int dir_pin;
    };

    void init(int step, int dir);

    void step();
    bool singleStep();

    void setTarget(long newDistance);
    long getPosition()
    {
        return this->position;
    };
    void setCurrentPosition(long newPosition)
    {
        this->position = newPosition;
    }
    // void setPosition(long x, long y, int speed);
    bool needsMoving();
    void stop();
    void forceStop();
    void setSpeed(long speed);

    volatile bool _isRunning;
    bool isRunning()
    {
        return this->_isRunning;
    }

    VoidCallback callback;
    void startTimer(float period);

    volatile long position = 0;
    long speed = 0;
    XYStepper stepper;
    // long lastStepAt = 0;

    // Lieb ramp
    long distance = 0;
    long distanceRun = 0;
    float delayPeriod = 0;

    void resetRamping()
    {
        this->distance = 0;
        this->distanceRun = 0;
        float delayPeriod = 0;
        this->speed = 0;
    }

    void clearStep();

    // inherited form directable
    void setDirection(StepDirection dir);
    virtual void clearDirection();
};