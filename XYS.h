#include <Arduino.h>
#if defined(ARDUINO_SAM_DUE)
    #include <DueTimer.h>
#endif
#if defined(ARDUINO_GIGA)
    #include "Portenta_H7_TimerInterrupt.h"
#endif

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

enum MainAxis {
    X,
    Y
};

typedef void (*VoidCallback)();

class XYS {
    public:
    void setCallback(VoidCallback callback);
    #if defined(ARDUINO_GIGA)
    void setTimer(Portenta_H7_Timer *timer);
    #endif
    #if defined(ARDUINO_SAM_DUE)
    void setTimer(DueTimer *timer);
    #endif
    void init(int stepX,int dirX,int stepY,int dirY);
    void step();
    void setPosition(long x, long y);
    long getX() 
    {
        return this->x;
    };
    long getY() 
    {
        return this->y;
    };
    void setCurrentPosition(long x, long y)
    {
        this->x = x;
        this->y = y;
    };
    // void setPosition(long x, long y, int speed);
    bool isRunning();
    void stop();

    VoidCallback callback;
    void startTimer(float period);


    StepDirection direction = StepDirection::FORWARD;
    // long targetX = 0;
    // long targetY = 0;
    long x = 0;
    long y = 0;
    long speed = 0;
    XYStepper stepperX;
    XYStepper stepperY;
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


    // Bresenham
    int dx;
    int dy;
    int sx;
    int sy;
    int err;
    MainAxis axis;

    void resetBresenham()
    {
        this->dx = 0;
        this->dy = 0;
        this->sx = 0;
        this->sy = 0;
        this->err = 0;
    };


    // Timer
    #if defined(ARDUINO_GIGA)
        Portenta_H7_Timer *timer;
    #endif
    #if defined(ARDUINO_SAM_DUE)
        DueTimer *timer;
    #endif
    
};