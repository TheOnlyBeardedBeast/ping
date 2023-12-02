#include "Paddle.h"
#include "utils.h"

#define CLAMP(value, minValue, maxValue) ((value) < (minValue) ? (minValue) : ((value) > (maxValue) ? (maxValue) : (value)))

#define ENCODER_RESOLUTION 600


Paddle::Paddle()
{
    this->direction = CW;
    // this->_pulseCount = 0;
    // this->_lastTime = 0;
    // this->_currentTime = 0;
    // this->_deltaTime = 0;
    this->smoother = DirectionSmoother(10);
}

void Paddle::initializeEncoder(byte A, byte B)
{
    this->_pinA = A;
    this->_pinB = B;

    pinMode(A, INPUT);
    pinMode(B, INPUT);
}

void Paddle::initializeStepper(AxisStepper* stepper)
{
    this->_stepper = stepper;
}

void Paddle::initCalibration()
{
    this->_stepper->setSpeed(CALIBRATION_SPEED);
}

void Paddle::calibratePosition(CalibrationPosition position)
{
    this->_stepper->setPosition(this->CALIBRATION_LIMITS[position]);

    while (!this->limitSwitchState[position]);
}

void Paddle::runCalibration()
{
    this->calibratePosition(CalibrationPosition::MIN);
    this->_stepper->setPosition(-SAFEZONE_WIDTH);


    this->calibratePosition(CalibrationPosition::MAX);
    this->max = this->_stepper->getPosition() - SAFEZONE_WIDTH;

    this->center();
}

void Paddle::center()
{
    this->_stepper->setPosition(this->max>>1);
}

void Paddle::runCenter()
{
    this->center();

    while (this->_stepper->isRunning());
}

void Paddle::readEncoder()
{
    if (this->readA() == this->readB())
    {
        this->direction = CW;
        smoother.smoothDirection(CW);
        this->_stepper->singleStep(AxisStepper::StepDirection::FORWARD);
        // if (smoother.getCurrentDirection() == CW && this->_stepper->direction == -1)
        // {
        //     // this->_stepper->moveTo(10000);
        // }
    }
    else
    {
        this->direction = CCW;
        smoother.smoothDirection(CCW);
        this->_stepper->singleStep(AxisStepper::StepDirection::BACKWARD);
        // if (smoother.getCurrentDirection() == CCW && this->_stepper->direction == 1)
        // {
        //     // this->_stepper->moveTo(-10000);
        // }
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
    this->_stepper->getPosition();
}

bool Paddle::needsToMove()
{
    this->_stepper->isRunning();
}

void Paddle::isrReadEncoder0()
{
    if (Paddle::instances[0]->readA() == Paddle::instances[0]->readB())
    {
        Paddle::instances[0]->direction = CW;
        // smoother.smoothDirection(CW);
        Paddle::instances[0]->_stepper->singleStep(AxisStepper::StepDirection::FORWARD);
        // if (smoother.getCurrentDirection() == CW && this->_stepper->direction == -1)
        // {
        //     // this->_stepper->moveTo(10000);
        // }
    }
    else
    {
        Paddle::instances[0]->direction = CCW;
        // smoother.smoothDirection(CCW);
        Paddle::instances[0]->_stepper->singleStep(AxisStepper::StepDirection::BACKWARD);
        // if (smoother.getCurrentDirection() == CCW && this->_stepper->direction == 1)
        // {
        //     // this->_stepper->moveTo(-10000);
        // }
    }
}

void Paddle::isrReadEncoder1()
{
    if (Paddle::instances[1]->readA() == Paddle::instances[1]->readB())
    {
        Paddle::instances[1]->direction = CW;
        // smoother.smoothDirection(CW);
        Paddle::instances[1]->_stepper->singleStep(AxisStepper::StepDirection::FORWARD);
        // if (smoother.getCurrentDirection() == CW && this->_stepper->direction == -1)
        // {
        //     // this->_stepper->moveTo(10000);
        // }
    }
    else
    {
        Paddle::instances[1]->direction = CCW;
        // smoother.smoothDirection(CCW);
        Paddle::instances[1]->_stepper->singleStep(AxisStepper::StepDirection::BACKWARD);
        // if (smoother.getCurrentDirection() == CCW && this->_stepper->direction == 1)
        // {
        //     // this->_stepper->moveTo(-10000);
        // }
    }
}

void Paddle::attachPaddles()
{
    attachInterrupt(
        digitalPinToInterrupt(
            Paddle::instances[0]->_pinA), Paddle::isrReadEncoder0, PinStatus::RISING);
    attachInterrupt(
        digitalPinToInterrupt(
            Paddle::instances[1]->_pinA), Paddle::isrReadEncoder1, PinStatus::RISING);
}

void Paddle::detachPaddles()
{
    detachInterrupt(digitalPinToInterrupt(Paddle::instances[0]->_pinA));
    detachInterrupt(digitalPinToInterrupt(Paddle::instances[1]->_pinA));
}