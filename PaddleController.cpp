#include "PaddleController.h"
#define CLAMP(value, minValue, maxValue) ((value) < (minValue) ? (minValue) : ((value) > (maxValue) ? (maxValue) : (value)))

#define ENCODER_RESOLUTION 2400
#define SAMPLING_TIME 100
#define SPEED 1000
#define ACCELERATION 10 * SPEED
#define CALIBRATION_SPEED 200

PaddleController::PaddleController()
{
    this->direction = CW;
    this->_pulseCount = 0;
    this->_lastTime = 0;
    this->_currentTime = 0;
    this->_deltaTime = 0;
    this->smoother = DirectionSmoother(10);
}

void PaddleController::initializeEncoder(byte A, byte B)
{
    this->_pinA = A;
    // this->_registerA = digitalPinToPort(A)->PIO_PDSR;
    // this->_registerB = digitalPinToPort(B)->PIO_PDSR;

    this->_pinB = B;
    // this->_bitMaskA = digitalPinToBitMask(A);
    // this->_bitMaskA = digitalPinToBitMask(B);

    pinMode(A, INPUT_PULLUP);
    pinMode(B, INPUT_PULLUP);
}

void PaddleController::initializeMotor(byte STEP, byte DIR)
{
    pinMode(STEP, OUTPUT);
    pinMode(DIR, OUTPUT);

    this->_stepper = AccelStepper(1, STEP, DIR);
    this->_stepper.setMaxSpeed(SPEED);
    this->_stepper.setAcceleration(ACCELERATION);
}

void PaddleController::initCalibration()
{
    this->_stepper.setSpeed(CALIBRATION_SPEED);
}

void PaddleController::runCalibration()
{
    //     if (MIN_LIMIT_CLICKED && this->limitMin == -1)
    //     {
    //         this->_stepper.stop();
    //         this->_stepper.setCurrentPosition(0);
    //         this->limitMin = this->_stepper.currentPosition();
    //         this->_stepper.setSpeed(-CALIBRATION_SPEED);
    //         goto RUN_CALIBRATION;
    //     }

    //     if (MAX_LIMIT_CLICKED && this->limitMax == -1)
    //     {
    //         this->_stepper.stop();
    //         this->limitMax = this->_stepper.currentPosition();
    //         this->_stepper.setSpeed(-CALIBRATION_SPEED);
    //     }
    // RUN_CALIBRATION:
    //     if (this->limitMin == -1 || this->limitMax == -1)
    //     {
    //         this->_stepper.runSpeed();
    //         return;
    //     }

    //     this->_stepper.stop();
}

void PaddleController::run()
{

    if (this->_deltaTime >= SAMPLING_TIME)
    {
        this->running = true;

        // Calculation for the new max speed
        double dTime = (double)this->_deltaTime;
        double dPulse = (double)this->_pulseCount;

        double newSpeed = (((10 / dTime) * (dTime * dPulse)) / ENCODER_RESOLUTION) * 100;
        newSpeed = CLAMP(newSpeed, 200, 800);

        this->speed = newSpeed;
        this->_stepper.setMaxSpeed(this->speed);

        this->_pulseCount = 0;
        this->_deltaTime = 0;
    }
    else if (this->running && millis() - this->_lastTime > SAMPLING_TIME + 10)
    {
        this->running = false;
        this->_stepper.stop();
        this->speed = 0;
    }

    this->_stepper.run();
}

void PaddleController::isrA()
{
    if (this->readA() == this->readB())
    {
        this->direction = CW;
        smoother.smoothDirection(CW);
        if (smoother.getCurrentDirection() == CW && this->_stepper.distanceToGo() <= 0)
        {
            this->_stepper.moveTo(10000);
        }
    }
    else
    {
        this->direction = CCW;
        smoother.smoothDirection(CCW);
        if (smoother.getCurrentDirection() == CCW && this->_stepper.distanceToGo() >= 0)
        {
            this->_stepper.moveTo(-10000);
        }
    }

    this->_pulseCount++;
    this->_currentTime = millis();
    if (_currentTime - _lastTime >= SAMPLING_TIME)
    {
        this->_deltaTime = _currentTime - _lastTime;
        this->_lastTime = _currentTime;
    }
}

void PaddleController::isrB()
{

    if (this->readB() != readA())
    {
        this->direction = CW;
        smoother.smoothDirection(CW);
        if (smoother.getCurrentDirection() == CW && this->_stepper.distanceToGo() <= 0)
        {
            this->_stepper.moveTo(10000);
        }
    }
    else
    {
        this->direction = CCW;
        smoother.smoothDirection(CCW);
        if (smoother.getCurrentDirection() == CCW && this->_stepper.distanceToGo() >= 0)
        {
            this->_stepper.moveTo(-10000);
        }
    }

    this->_pulseCount++;
    this->_currentTime = millis();
    if (_currentTime - _lastTime >= SAMPLING_TIME)
    {
        this->_deltaTime = _currentTime - _lastTime;
        this->_lastTime = _currentTime;
    }
}

int PaddleController::readA()
{
    return digitalRead(this->_pinA);
    // return bitRead(this->_registerA, this->_bitMaskA);
}

int PaddleController::readB()
{
    return digitalRead(this->_pinB);
    // return bitRead(this->_registerB, this->_bitMaskB);
}
