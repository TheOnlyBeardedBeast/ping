#include <Arduino.h>
#if defined(ARDUINO_SAM_DUE)
    #include <DueTimer.h>
#endif
#if defined(ARDUINO_GIGA)
    #include "Portenta_H7_TimerInterrupt.h"
#endif



typedef void (*VoidCallback)();

class AxisStepper {
    public:

    struct XYStepper {
    int step_pin;
    int dir_pin;
    // TODO: store pin mask for step
    // TODO: store pin mask for dir
    };

    enum StepDirection {
        FORWARD = 1,
        BACKWARD = -1,
    };

    void setCallback(VoidCallback callback);
    #if defined(ARDUINO_GIGA)
    void setTimer(Portenta_H7_Timer *timer);
    #endif
    #if defined(ARDUINO_SAM_DUE)
    void setTimer(DueTimer *timer);
    #endif
    void init(int step,int dir);
    void step();
    void setPosition(long newDistance);
    long getPosition() 
    {
        return this->position;
    };
    void setCurrentPosition(long newPosition)
    {
        this->position = newPosition;
    }
    // void setPosition(long x, long y, int speed);
    bool isRunning();
    void stop();
    void forceStop();

    VoidCallback callback;
    void startTimer(float period);


    StepDirection direction = StepDirection::FORWARD;

    long position = 0;
    long speed = 0;
    XYStepper stepper;
    // long lastStepAt = 0;

    // Lieb ramp
    long acceleration = 0;
    long distance = 0;
    long distanceRun = 0;
    long accelDistance = 0;
    long deccelDistance = 0;
    float multiplier = 0;
    float delayPeriod = 0;

    void resetRamping() {
        this->acceleration = 0;
        this->distance = 0;
        this->distanceRun = 0;
        this->accelDistance = 0;
        this->deccelDistance = 0;
        this->multiplier = 0;
        float delayPeriod = 0;
    }


    // Timer
    #if defined(ARDUINO_GIGA)
        Portenta_H7_Timer *timer;
    #endif
    #if defined(ARDUINO_SAM_DUE)
        DueTimer *timer;
    #endif
    
};