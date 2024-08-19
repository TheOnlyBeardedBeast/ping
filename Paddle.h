#pragma once
#include <Arduino.h>
#include "AxisStepper.h"

#define SENSITIVITY 3

enum Direction
{
    CW,
    CCW,
};

enum CalibrationPosition
{
    MIN = 0,
    MAX = 1
};

using CallbackFunction = void (*)(int);

class Paddle
{
public:
    // variables
    int count;
    byte id;
    int modulatorA = 0;
    int modulatorB = 0;
    uint32_t lastStep = 0;
    Direction direction = CW;
    double speed = 0;
    int limitMin = 0;
    int limitMax = 2000;
    int limitSwitchState[2] = {false, false};
    unsigned int CALIBRATION_LIMITS[2] = {-10000, 10000};
    unsigned int max = 0;
    uint32_t lastRunA;
    uint32_t lastRunB;
    AxisStepper *_stepper = nullptr;

    // constructors
    Paddle();

    // methods
    void initializeEncoder(int A, int B);
    void initializeStepper(AxisStepper *stepper);
    bool canShoot(long x);

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
    // variables
    int _pinA;
    // RoReg _registerA;
    // int _bitMaskA;
    int _pinB;
    // RoReg _registerB;
    // int _bitMaskB;
    byte stepIndex = 0;

    bool running = false;

    // unsigned int _pulseCount = 0;
    // unsigned long _lastTime = 0;
    // unsigned long _currentTime = 0;
    // unsigned long _deltaTime = 0;

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
    void setDirectionListener(DirectionCallback callback)
    {
        this->onDirectionChange = callback;
    }
    void setStepListener(StepCallback callback)
    {
        this->onStepChange = callback;
    }
};