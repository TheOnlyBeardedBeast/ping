#include "Paddle.h"
#include "utils.h"
#include "GameConfig.h"

#define CLAMP(value, minValue, maxValue) ((value) < (minValue) ? (minValue) : ((value) > (maxValue) ? (maxValue) : (value)))

#define ENCODER_RESOLUTION 600
// #include "ClearTarget.h"

// extern ClearTarget clearTimes[6];

Paddle *Paddle::instances[2];
bool Paddle::calibrated = false;

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
    pinMode(A, INPUT_PULLUP);
    pinMode(B, INPUT_PULLUP);

    this->_pinA = A;
    this->_pinB = B;
}

void Paddle::initializeStepper(AxisStepper *stepper)
{
    this->_stepper = stepper;
}

void Paddle::center()
{
    this->_stepper->setTarget(this->max >> 1);
}

void Paddle::runCenter()
{
    if (this->_stepper->position < 1000)
    {
        this->_stepper->setDirection(AxisStepper::StepDirection::FORWARD);
    }
    else
    {
        this->_stepper->setDirection(AxisStepper::StepDirection::BACKWARD);
    }

    while (this->_stepper->position != 1000)
    {
        this->_stepper->singleStep();
        delayMicroseconds(500);
    }
}

void Paddle::stop()
{
    this->_stepper->stop();
}

// void Paddle::singleStep()
// {
// if (clearTimes[this->id + 4].enabled)
// {
//     return;
// }

// uint32_t currentStep = micros();
// uint32_t minTime = this->lastStep + 1000;

// if (this->lastStep > currentStep - 1000)
// {
//     clearTimes[this->id + 4].enabled = true;
//     clearTimes[this->id + 4].time = minTime;
// }
// else
// {
//     this->_stepper->singleStep();
//     this->lastStep = currentStep;
// }
// }

void Paddle::singleStep()
{
    if (this->stepIndex == 0)
    {
        this->_stepper->singleStep();
    }

    this->stepIndex = (this->stepIndex + 1) % 6;
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

void Paddle::clearSingleStep()
{
    this->_stepper->singleStep();
}

void Paddle::isrReadEncoder0()
{
    if (!Paddle::instances[0]->readB())
    {
        Paddle::instances[0]->_stepper->setDirection(AxisStepper::StepDirection::FORWARD);

        Paddle::instances[0]->singleStep();
    }
    else
    {
        Paddle::instances[0]->_stepper->setDirection(AxisStepper::StepDirection::BACKWARD);

        Paddle::instances[0]->singleStep();
    }
}

void Paddle::isrReadEncoder01()
{

    if (Paddle::instances[0]->readA())
    {
        Paddle::instances[0]->_stepper->setDirection(AxisStepper::StepDirection::FORWARD);

        Paddle::instances[0]->singleStep();
    }
    else
    {
        Paddle::instances[0]->_stepper->setDirection(AxisStepper::StepDirection::BACKWARD);

        Paddle::instances[0]->singleStep();
    }
}

void Paddle::isrReadEncoder10()
{
    if (!Paddle::instances[1]->readB())
    {
        Paddle::instances[1]->_stepper->setDirection(AxisStepper::StepDirection::FORWARD);

        Paddle::instances[1]->singleStep();
    }
    else
    {
        Paddle::instances[1]->_stepper->setDirection(AxisStepper::StepDirection::BACKWARD);

        Paddle::instances[1]->singleStep();
    }
}

void Paddle::isrReadEncoder11()
{
    if (Paddle::instances[1]->readA())
    {
        Paddle::instances[1]->_stepper->setDirection(AxisStepper::StepDirection::FORWARD);

        Paddle::instances[1]->singleStep();
    }
    else
    {
        Paddle::instances[1]->_stepper->setDirection(AxisStepper::StepDirection::BACKWARD);

        Paddle::instances[1]->singleStep();
    }
}

void Paddle::calibrate()
{
    Paddle *p1 = Paddle::instances[0];
    Paddle *p2 = Paddle::instances[1];

    p1->_stepper->setDirection(AxisStepper::StepDirection::BACKWARD);
    p2->_stepper->setDirection(AxisStepper::StepDirection::BACKWARD);

    while (!p1->_stepper->calibrated || !p2->_stepper->calibrated)
    {
        if (!digitalRead(LS3))
        {
            p1->_stepper->calibrated = true;
        }

        if (!digitalRead(LS4))
        {
            p2->_stepper->calibrated = true;
        }

        if (!p1->_stepper->calibrated)
        {
            p1->_stepper->singleStep();
        }

        if (!p2->_stepper->calibrated)
        {
            p2->_stepper->singleStep();
        }

        delayMicroseconds(4000);
    }

    delay(1000);

    p1->_stepper->setDirection(AxisStepper::StepDirection::FORWARD);
    p2->_stepper->setDirection(AxisStepper::StepDirection::FORWARD);

    delayMicroseconds(5);

    for (size_t i = 0; i < 10; i++)
    {
        p1->_stepper->singleStep();
        p2->_stepper->singleStep();
        delayMicroseconds(4000);
    }

    p1->_stepper->setCurrentPosition(0);
    p2->_stepper->setCurrentPosition(0);

    Paddle::calibrated = true;
}

void Paddle::centerAll()
{
    Paddle *p1 = Paddle::instances[0];
    Paddle *p2 = Paddle::instances[1];

    bool centered[2] = {false, false};

    while (!centered[0] || !centered[1])
    {
        if (!centered[0] && p1->getPosition() != 1000)
        {
            p1->_stepper->singleStep();
        }
        else
        {
            centered[0] = true;
        }

        if (!centered[1] && p2->getPosition() != 1000)
        {
            p2->_stepper->singleStep();
        }
        else
        {
            centered[1] = true;
        }

        delayMicroseconds(500);
    }
}

void Paddle::attachPaddles()
{

    attachInterrupt(
        digitalPinToInterrupt(
            Paddle::instances[0]->_pinA),
        Paddle::isrReadEncoder0, RISING);
    attachInterrupt(
        digitalPinToInterrupt(
            Paddle::instances[0]->_pinB),
        Paddle::isrReadEncoder01, RISING);
    attachInterrupt(
        digitalPinToInterrupt(
            Paddle::instances[1]->_pinA),
        Paddle::isrReadEncoder10, RISING);
    attachInterrupt(
        digitalPinToInterrupt(
            Paddle::instances[1]->_pinB),
        Paddle::isrReadEncoder11, RISING);
}

void Paddle::detachPaddles()
{
    detachInterrupt(digitalPinToInterrupt(Paddle::instances[0]->_pinA));
    detachInterrupt(digitalPinToInterrupt(Paddle::instances[1]->_pinA));
    detachInterrupt(digitalPinToInterrupt(Paddle::instances[0]->_pinB));
    detachInterrupt(digitalPinToInterrupt(Paddle::instances[1]->_pinB));
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