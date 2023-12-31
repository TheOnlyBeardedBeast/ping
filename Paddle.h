#include <Arduino.h>
#include <AccelStepper.h>
#include "DirectionSmoother.h"

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
    Direction direction = CW;
    double speed = 0;
    int limitMin = -1;
    int limitMax = -1;
    byte limitSwitchState[2] = {false,false};
    unsigned int CALIBRATION_LIMITS[2] = {-10000,10000};
    unsigned int max = 0;

    // constructors
    Paddle();

    // methods
    void initializeEncoder(byte A, byte B);
    void initializeStepper(AccelStepper* stepper);
    void initCalibration();
    void runCalibration();
    void run();
    void isrA();
    void isrB();
    void stop();
    void center();
    void runCenter();
    long getPosition();
    bool needsToMove();

private:
    // variables
    byte _pinA;
    // RoReg _registerA;
    // int _bitMaskA;
    byte _pinB;
    // RoReg _registerB;
    // int _bitMaskB;
    AccelStepper *_stepper = nullptr;
    bool running = false;
    DirectionSmoother smoother;

    unsigned int _pulseCount = 0;
    unsigned long _lastTime = 0;
    unsigned long _currentTime = 0;
    unsigned long _deltaTime = 0;

    // methods

    int readA();
    int readB();
    void calibratePosition(CalibrationPosition postion);
};

AccelStepper* initializeStepper(byte STEP, byte DIR);