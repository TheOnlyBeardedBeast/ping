#pragma once

#include <Arduino.h>
#include "Models/Directable.h"
#if defined(ARDUINO_GIGA)
#include "Portenta_H7_TimerInterrupt.h"
#elif defined(ARDUINO_SAM_DUE)
#include <DueTimer.h>
#endif
#include "helpers.h"
#include "DueWriteFast.h"

struct Target
{
    int x = 0;
    int y = 0;
    bool needsRun = false;
};

struct XYStepper
{
    int step_pin;
    int dir_pin;
    uint16_t step_mask;
    uint16_t dir_mask;
    // TODO: store pin mask for step
    // TODO: store pin mask for dir
};

enum MainAxis
{
    X,
    Y
};

class XYS
{
    static XYS *instance;
    static void ballIsr();

public:
    void moveWhile(PinStatus motor1, PinStatus motor2, unsigned short speed, BoolCallback condition);

#if defined(ARDUINO_GIGA)
    GPIO_TypeDef *port;
#elif defined(ARDUINO_SAM_DUE)
    Pio *port;
#endif

#if defined(ARDUINO_GIGA)
    void setTimer(Portenta_H7_Timer *timer);
#elif defined(ARDUINO_SAM_DUE)
    void setTimer(DueTimer *timer);
#endif

    void init(int stepX, int dirX, int stepY, int dirY);
    void step();
    void setPosition(long x, long y, int moveSpeed, int startSpeed, int endSpeed);
    long getX()
    {
        return this->x;
    };
    long getY()
    {
        return this->y;
    };
    void setCurrentPosition(long _x, long _y)
    {
        this->x = _x;
        this->y = _y;
    };
    // void setPosition(long x, long y, int speed);
    bool needsMoving();
    void stop();
    void stopNow();

    void startTimer(float period);

    volatile bool moving = false;
    bool isMoving()
    {
        return this->moving;
    }
    StepDirection direction = StepDirection::NONE;
    void setDirection(StepDirection _direction);
    // long targetX = 0;
    // long targetY = 0;
    volatile long x = 0;
    volatile long y = 0;
    long speed = 0;
    long targetSpeed = 0;
    XYStepper stepperX;
    XYStepper stepperY;
    // long lastStepAt = 0;

    // Reamp
    long startSpeed = 0;
    long endSpeed = 0;
    // Lieb ramp
    long acceleration = 0;
    long distance = 0;
    long distanceRun = 0;
    long accelDistance = 0;
    long deccelDistance = 0;
    float multiplier = 0;
    float delayPeriod = 0;
    bool cleared = true;

    void resetRamping()
    {
        this->acceleration = 0;
        this->distance = 0;
        this->distanceRun = 0;
        this->accelDistance = 0;
        this->deccelDistance = 0;
        this->multiplier = 0;
        this->delayPeriod = 0;
        this->speed = 0;
        this->targetSpeed = 0;
        this->cleared = true;
    }

    void printRamp()
    {
        Serial.print("Acceleration:");
        Serial.println(this->acceleration);
        Serial.print("Distance:");
        Serial.println(this->distance);
        Serial.print("DistanceRun:");
        Serial.println(this->distanceRun);
        Serial.print("Acceldistance:");
        Serial.println(this->accelDistance);
        Serial.print("Decceldistance:");
        Serial.println(this->deccelDistance);
        Serial.print("Multiplier:");
        Serial.println(this->multiplier);
        Serial.print("DelayPeriod:");
        Serial.println(this->delayPeriod);
        Serial.print("Speed:");
        Serial.println(this->speed);
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

    void printBresenham()
    {
        Serial.print("Dx:");
        Serial.println(this->dx);
        Serial.print("dy:");
        Serial.println(this->dy);
        Serial.print("sx:");
        Serial.println(this->sx);
        Serial.print("sy:");
        Serial.println(this->sy);
        Serial.print("err:");
        Serial.println(this->err);
    }

    void singleStep();
    void clearStep();
    void clearDirection() {};

    Target nexTarget;
    void setNextTarget(int _x, int _y)
    {
        nexTarget.x = _x;
        nexTarget.y = _y;
        nexTarget.needsRun = true;
    }

// Timer
#if defined(ARDUINO_GIGA)
    Portenta_H7_Timer *timer;
#elif defined(ARDUINO_SAM_DUE)
    DueTimer *timer;
#endif
};