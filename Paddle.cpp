#include "Paddle.h"
#include "utils.h"
#include "GameConfig.h"

#define CLAMP(value, minValue, maxValue) ((value) < (minValue) ? (minValue) : ((value) > (maxValue) ? (maxValue) : (value)))

#define ENCODER_RESOLUTION 600

Paddle *Paddle::instances[2];

// int Paddle::count = 0;

Paddle::Paddle()
{
    this->direction = CW;
    // this->_pulseCount = 0;
    // this->_lastTime = 0;
    // this->_currentTime = 0;
    // this->_deltaTime = 0;
    this->smoother = DirectionSmoother();
    this->lastRunA = 0;
    this->lastRunB = 0;
}

void Paddle::initializeEncoder(int A, int B)
{
    pinMode(A,INPUT_PULLUP);
    pinMode(B,INPUT_PULLUP);

    this->_pinA = A;
    this->_pinB = B;
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
    // int32_t stamp = micros();
    // if(stamp - Paddle::instances[0]->lastRunA < 500)
    // {
    //     Paddle::instances[0]->lastRunA = stamp;
    //     return;
    // }

    // Paddle::instances[0]->lastRunA = stamp;

    // Paddle::instances[0]->modulatorA = (Paddle::instances[0]->modulatorA+1)%SENSITIVITY;

    // if(Paddle::instances[0]->modulatorA)
    // {
    //     return;
    // }
    
    if(!Paddle::instances[0]->readB())
    {
        // AxisStepper::StepDirection dir = Paddle::instances[0]->smoother.smoothDirection(AxisStepper::StepDirection::FORWARD);

        Paddle::instances[0]->_stepper->setDirection(AxisStepper::StepDirection::FORWARD);
        // Paddle::instances[0]->_stepper->setDirection(dir);
        
        Paddle::instances[0]->_stepper->singleStep();
        Paddle::instances[0]->count -= 1;
    }
    else
    {
        // AxisStepper::StepDirection dir = Paddle::instances[0]->smoother.smoothDirection(AxisStepper::StepDirection::BACKWARD);

        Paddle::instances[0]->_stepper->setDirection(AxisStepper::StepDirection::BACKWARD);
        // Paddle::instances[0]->_stepper->setDirection(dir);

        Paddle::instances[0]->_stepper->singleStep();
        Paddle::instances[0]->count += 1;
    }
}

void Paddle::isrReadEncoder01()
{
    // int32_t stamp = micros();
    // if(stamp - Paddle::instances[0]->lastRunB < 500)
    // {
    //     Paddle::instances[0]->lastRunB = stamp;
    //     return;
    // }

    // Paddle::instances[0]->lastRunB = stamp;

    // Paddle::instances[0]->modulatorB = (Paddle::instances[0]->modulatorB+1)%SENSITIVITY;

    // if(Paddle::instances[0]->modulatorB)
    // {
    //     return;
    // }

    if(Paddle::instances[0]->readA())
    {
        // AxisStepper::StepDirection dir = Paddle::instances[0]->smoother.smoothDirection(AxisStepper::StepDirection::FORWARD);

        Paddle::instances[0]->_stepper->setDirection(AxisStepper::StepDirection::FORWARD);
        // Paddle::instances[0]->_stepper->setDirection(dir);
        
        Paddle::instances[0]->_stepper->singleStep();
        Paddle::instances[0]->count -= 1;
    }
    else
    {
        // AxisStepper::StepDirection dir = Paddle::instances[0]->smoother.smoothDirection(AxisStepper::StepDirection::BACKWARD);

        Paddle::instances[0]->_stepper->setDirection(AxisStepper::StepDirection::BACKWARD);
        // Paddle::instances[0]->_stepper->setDirection(dir);

        Paddle::instances[0]->_stepper->singleStep();
        Paddle::instances[0]->count += 1;
    }
}

void Paddle::isrReadEncoder10()
{
    // if (Paddle::instances[1]->readA() == Paddle::instances[1]->readB())
    if (!Paddle::instances[1]->readB())
    {
        Paddle::instances[1]->_stepper->setDirection(AxisStepper::StepDirection::FORWARD);
        
        Paddle::instances[1]->_stepper->singleStep();
    }
    else
    {
        Paddle::instances[1]->_stepper->setDirection(AxisStepper::StepDirection::BACKWARD);

        Paddle::instances[1]->_stepper->singleStep();
    }
}

void Paddle::isrReadEncoder11()
{
    if (Paddle::instances[1]->readA())
    {
        Paddle::instances[1]->_stepper->setDirection(AxisStepper::StepDirection::FORWARD);
        
        Paddle::instances[1]->_stepper->singleStep();
    }
    else
    {
        Paddle::instances[1]->_stepper->setDirection(AxisStepper::StepDirection::BACKWARD);

        Paddle::instances[1]->_stepper->singleStep();
    }
}

void Paddle::attachPaddles()
{
    
    attachInterrupt(
        digitalPinToInterrupt(
            Paddle::instances[0]->_pinA), Paddle::isrReadEncoder0, RISING);
            attachInterrupt(
        digitalPinToInterrupt(
            Paddle::instances[0]->_pinB), Paddle::isrReadEncoder01, RISING);
    attachInterrupt(
        digitalPinToInterrupt(
            Paddle::instances[1]->_pinA), Paddle::isrReadEncoder10, RISING);
            attachInterrupt(
        digitalPinToInterrupt(
            Paddle::instances[1]->_pinB), Paddle::isrReadEncoder11, RISING);
}

void Paddle::detachPaddles()
{
    detachInterrupt(digitalPinToInterrupt(Paddle::instances[0]->_pinA));
    // detachInterrupt(digitalPinToInterrupt(Paddle::instances[1]->_pinA));
}

bool Paddle::canShoot(long x)
{
    long currentPosition = this->getPosition();

    long ballStart = x - BALL_HALF;
    long ballEnd = x + BALL_HALF;

    long paddleStart = currentPosition - PADDLE_HALF;
    long paddleEnd = currentPosition + PADDLE_HALF;

    return ballStart <= paddleEnd && ballEnd >= paddleStart;
}