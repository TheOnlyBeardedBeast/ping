#include "Paddle.h"
#include "utils.h"

#define CLAMP(value, minValue, maxValue) ((value) < (minValue) ? (minValue) : ((value) > (maxValue) ? (maxValue) : (value)))

#define ENCODER_RESOLUTION 2400
#define SAMPLING_TIME 100


Paddle::Paddle()
{
    this->direction = CW;
    this->_pulseCount = 0;
    this->_lastTime = 0;
    this->_currentTime = 0;
    this->_deltaTime = 0;
    this->smoother = DirectionSmoother(10);
}

void Paddle::initializeEncoder(byte A, byte B)
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

void Paddle::initializeStepper(AccelStepper* stepper)
{
    this->_stepper = stepper;
    this->_stepper->setMaxSpeed(SPEED);
    this->_stepper->setAcceleration(ACCELERATION);
}

AccelStepper *initializeStepper(byte STEP, byte DIR)
{
    pinMode(STEP, OUTPUT);
    pinMode(DIR, OUTPUT);

    AccelStepper stepper = AccelStepper(1, STEP, DIR);

    stepper.setMaxSpeed(SPEED);
    stepper.setAcceleration(ACCELERATION);

    return &stepper;
}

void Paddle::initCalibration()
{
    this->_stepper->setSpeed(CALIBRATION_SPEED);
}

void Paddle::calibratePosition(CalibrationPosition position)
{
    this->_stepper->moveTo(this->CALIBRATION_LIMITS[position]);
    this->_stepper->setAcceleration(ACCELERATION);
    this->_stepper->setMaxSpeed(CALIBRATION_SPEED);

    while (!this->limitSwitchState[position])
    {
        this->_stepper->run();
    }

    this->_stepper->stop();
}

void Paddle::runCalibration()
{
    this->calibratePosition(CalibrationPosition::MIN);
    this->_stepper->setCurrentPosition(-SAFEZONE_WIDTH);


    this->calibratePosition(CalibrationPosition::MAX);
    this->max = this->_stepper->currentPosition() - SAFEZONE_WIDTH;

    this->center();
}

void Paddle::center()
{
    this->_stepper->moveTo(this->max>>1);

    while (this->_stepper->distanceToGo()!=0)
    {
        this->_stepper->run();
    }

    this->_stepper->stop();
}

void Paddle::run()
{

    if (this->_deltaTime >= SAMPLING_TIME)
    {
        this->running = true;

        // Calculation for the new max speed
        double dTime = (double)this->_deltaTime;
        double dPulse = (double)this->_pulseCount;

        double newSpeed = (((10 / dTime) * (dTime * dPulse)) / ENCODER_RESOLUTION) * 100;
        newSpeed = CLAMP(newSpeed, 200, 800);

        if (newSpeed != this->speed)
        {
            this->speed = newSpeed;
            this->_stepper->setMaxSpeed(this->speed);
        }

        this->_pulseCount = 0;
        this->_deltaTime = 0;
    }
    else if (this->running && millis() - this->_lastTime > SAMPLING_TIME + 10)
    {
        this->running = false;
        this->_stepper->stop();
        this->speed = 0;
    }

    this->_stepper->run();
}

void Paddle::isrA()
{
    if (this->readA() == this->readB())
    {
        this->direction = CW;
        smoother.smoothDirection(CW);
        if (smoother.getCurrentDirection() == CW && this->_stepper->distanceToGo() <= 0)
        {
            this->_stepper->moveTo(10000);
        }
    }
    else
    {
        this->direction = CCW;
        smoother.smoothDirection(CCW);
        if (smoother.getCurrentDirection() == CCW && this->_stepper->distanceToGo() >= 0)
        {
            this->_stepper->moveTo(-10000);
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

void Paddle::isrB()
{

    if (this->readB() != readA())
    {
        this->direction = CW;
        smoother.smoothDirection(CW);
        if (smoother.getCurrentDirection() == CW && this->_stepper->distanceToGo() <= 0)
        {
            this->_stepper->moveTo(10000);
        }
    }
    else
    {
        this->direction = CCW;
        smoother.smoothDirection(CCW);
        if (smoother.getCurrentDirection() == CCW && this->_stepper->distanceToGo() >= 0)
        {
            this->_stepper->moveTo(-10000);
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

void Paddle::stop()
{
    this->_stepper->stop();
}

int Paddle::readA()
{
    return digitalRead(this->_pinA);
    // return bitRead(this->_registerA, this->_bitMaskA);
}

int Paddle::readB()
{
    return digitalRead(this->_pinB);
    // return bitRead(this->_registerB, this->_bitMaskB);
}

long Paddle::getPosition()
{
    this->_stepper->currentPosition();
}
