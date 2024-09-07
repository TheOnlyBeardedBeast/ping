#pragma once
#include <Arduino.h>
#include "AxisStepper.h"

#define PADDLE_SENSITIVITY 8
#define PADDLE_LIMIT 980
#define PADDLE_CENTER 490

using CallbackFunction = void (*)(int);

class Paddle
{
public:
    // variables
    byte id;
    AxisStepper *_stepper = nullptr;

    // constructors
    Paddle();

    // methods
    void initializeEncoder(int A, int B);
    void initializeStepper(AxisStepper *stepper);
    byte canShoot(long x);

    void stop();
    void center();
    void runCenter();
    long getPosition();
    long getCenterRelativePosition();
    bool needsToMove();
    void clearSingleStep();

    static Paddle *instances[2];
    static void attachPaddles();
    static void detachPaddles();
    static void isrReadEncoder0();
    static void isrReadEncoder01();
    static void isrReadEncoder10();
    static void isrReadEncoder11();
    static void calibrate();
    static bool calibrated;
    static void centerAll();

private:
    int _pinA;
    int _pinB;
    byte stepIndex = 0;

    // methods
    void setDirection(StepDirection _direction);
    void singleStep();
    int readA();
    int readB();

    // callbacks
    typedef void (*DirectionCallback)(StepDirection);
    typedef void (*StepCallback)();
    DirectionCallback onDirectionChange = NULL;
    StepCallback onStepChange = NULL;

public:
    void subscribe(DirectionCallback directionCallback, StepCallback stepCallback)
    {
        this->onDirectionChange = directionCallback;
        this->onStepChange = stepCallback;
    }

    void unsubScribe()
    {
        this->onDirectionChange = NULL;
        this->onStepChange = NULL;
    }
};