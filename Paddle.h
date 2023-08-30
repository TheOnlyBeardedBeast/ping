#include <Arduino.h>
#include <AccelStepper.h>
#include "DirectionSmoother.h"

enum Direction
{
    CW,
    CCW,
};

using CallbackFunction = void (*)(int);

class Paddle
{
public:
    // variables
    Direction direction = CW;
    double speed = 0;
    int limitMin = -1;
    int limitMax = -1;

    // constructors
    Paddle();

    // methods
    void initializeEncoder(byte A, byte B);
    void initializeStepper(byte STEP, byte DIRECTION);
    void initCalibration();
    void runCalibration();
    void run();
    void isrA();
    void isrB();
    void stop();

private:
    // variables
    byte _pinA;
    // RoReg _registerA;
    // int _bitMaskA;
    byte _pinB;
    // RoReg _registerB;
    // int _bitMaskB;
    AccelStepper _stepper;
    bool running = false;
    DirectionSmoother smoother;

    unsigned int _pulseCount = 0;
    unsigned long _lastTime = 0;
    unsigned long _currentTime = 0;
    unsigned long _deltaTime = 0;

    // methods

    int readA();
    int readB();
};