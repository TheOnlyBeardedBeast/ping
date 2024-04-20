#include <Arduino.h>
#include "DirectionSmoother.h"
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
    int modulatorA = 0;
    int modulatorB = 0;
    Direction direction = CW;
    double speed = 0;
    int limitMin = -1;
    int limitMax = -1;
    int limitSwitchState[2] = {false,false};
    unsigned int CALIBRATION_LIMITS[2] = {-10000,10000};
    unsigned int max = 0;
    uint32_t lastRunA;
    uint32_t lastRunB;

    // constructors
    Paddle();

    // methods
    void initializeEncoder(int A, int B);
    void initializeStepper(AxisStepper* stepper);
    bool canShoot(long x);
    void initCalibration();
    void runCalibration();
    void stop();
    void center();
    void runCenter();
    long getPosition();
    bool needsToMove();

    static Paddle * instances[2];
    static void attachPaddles();
    static void detachPaddles();
    static void isrReadEncoder0();
    static void isrReadEncoder01();
    static void isrReadEncoder1();

private:
    // variables
    int _pinA;
    // RoReg _registerA;
    // int _bitMaskA;
    int _pinB;
    // RoReg _registerB;
    // int _bitMaskB;
    AxisStepper *_stepper = nullptr;
    bool running = false;
    DirectionSmoother smoother;

    // unsigned int _pulseCount = 0;
    // unsigned long _lastTime = 0;
    // unsigned long _currentTime = 0;
    // unsigned long _deltaTime = 0;

    // methods

    int readA();
    int readB();
    void calibratePosition(CalibrationPosition postion);
};