#include "Paddle.h"
#include "utils.h"
#include "GameConfig.h"

#define CLAMP(value, minValue, maxValue) ((value) < (minValue) ? (minValue) : ((value) > (maxValue) ? (maxValue) : (value)))

#define ENCODER_RESOLUTION 600
#include "ClearTarget.h"

extern void clearSteps();

// extern ClearTarget clearTimes[6];

Paddle *Paddle::instances[2];
bool Paddle::calibrated = false;

// int Paddle::count = 0;

Paddle::Paddle()
{
    // this->_pulseCount = 0;
    // this->_lastTime = 0;
    // this->_currentTime = 0;
    // this->_deltaTime = 0;
    // this->lastRunA = 0;
    // this->lastRunB = 0;
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
    this->_stepper->setTarget(PADDLE_CENTER);
}

void Paddle::runCenter()
{
    if (this->_stepper->position < PADDLE_CENTER)
    {
        this->_stepper->setDirection(StepDirection::FORWARD);
    }
    else
    {
        this->_stepper->setDirection(StepDirection::BACKWARD);
    }

    while (this->_stepper->position != PADDLE_CENTER)
    {
        this->_stepper->singleStep();
        delayMicroseconds(500);
    }
}

void Paddle::stop()
{
    this->_stepper->stop();
}

void Paddle::setDirection(StepDirection _direction)
{
    this->_stepper->setDirection(_direction);
    if (this->onDirectionChange)
    {
        this->onDirectionChange(_direction);
    }
}

void Paddle::singleStep()
{
    if (this->stepIndex == 0)
    {

        if (this->_stepper->singleStep() && this->onStepChange)
        {
            this->onStepChange();
        }
    }

    this->stepIndex = (this->stepIndex + 1) % PADDLE_SENSITIVITY;
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
    return this->_stepper->getPosition();
}

long Paddle::getCenterRelativePosition()
{
    return this->getPosition() - PADDLE_CENTER;
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
        Paddle::instances[0]->setDirection(StepDirection::FORWARD);

        Paddle::instances[0]->singleStep();
    }
    else
    {
        Paddle::instances[0]->setDirection(StepDirection::BACKWARD);

        Paddle::instances[0]->singleStep();
    }
}

void Paddle::isrReadEncoder01()
{

    if (Paddle::instances[0]->readA())
    {
        Paddle::instances[0]->setDirection(StepDirection::FORWARD);

        Paddle::instances[0]->singleStep();
    }
    else
    {
        Paddle::instances[0]->setDirection(StepDirection::BACKWARD);

        Paddle::instances[0]->singleStep();
    }
}

void Paddle::isrReadEncoder10()
{
    if (!Paddle::instances[1]->readB())
    {
        Paddle::instances[1]->setDirection(StepDirection::FORWARD);

        Paddle::instances[1]->singleStep();
    }
    else
    {
        Paddle::instances[1]->setDirection(StepDirection::BACKWARD);

        Paddle::instances[1]->singleStep();
    }
}

void Paddle::isrReadEncoder11()
{
    if (Paddle::instances[1]->readA())
    {
        Paddle::instances[1]->setDirection(StepDirection::FORWARD);

        Paddle::instances[1]->singleStep();
    }
    else
    {
        Paddle::instances[1]->setDirection(StepDirection::BACKWARD);

        Paddle::instances[1]->singleStep();
    }
}

void Paddle::calibrate()
{
    Paddle *p1 = Paddle::instances[0];
    Paddle *p2 = Paddle::instances[1];

    p1->setDirection(StepDirection::BACKWARD);
    p2->setDirection(StepDirection::BACKWARD);

    delayMicroseconds(20);
    clearSteps();

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

        delayMicroseconds(20);
        clearSteps();

        delay(4);
    }

    delay(1000);

    p1->setDirection(StepDirection::FORWARD);
    p2->setDirection(StepDirection::FORWARD);

    delayMicroseconds(5);

    for (size_t i = 0; i < 10; i++)
    {
        p1->_stepper->singleStep();
        p2->_stepper->singleStep();
        delay(4);
        clearSteps();
        delayMicroseconds(3);
    }

    p1->_stepper->setCurrentPosition(0);
    p2->_stepper->setCurrentPosition(0);

    Paddle::calibrated = true;
}

void Paddle::centerAll()
{
    Paddle *p1 = Paddle::instances[0];
    Paddle *p2 = Paddle::instances[1];

    p1->_stepper->setTarget(PADDLE_CENTER);
    p2->_stepper->setTarget(PADDLE_CENTER);
    delay(1);
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

byte Paddle::canShoot(long ballPos)
{
    long paddlePos = this->getCenterRelativePosition();

    long paddleLeftEdge = paddlePos - PADDLE_WIDTH_HALF;
    long paddleRightEdge = paddlePos + PADDLE_WIDTH_HALF;
    long ballLeftEdge = ballPos - BALL_WIDTH_HALF;
    long ballRightEdge = ballPos + BALL_WIDTH_HALF;

    long hitzone = PADDLE_WIDTH_HALF + BALL_WIDTH_HALF;

    // Check if the ball is within the paddle's bounds
    if (ballRightEdge >= paddleLeftEdge && ballLeftEdge <= paddleRightEdge)
    {
        // Calculate the difference in positions
        double relativePosition = (ballPos - paddlePos);

        // Map the relative position to an angle between 30 and 150 degrees
        return round(map(relativePosition, -hitzone, hitzone, MIN_ANGLE_MUL, MAX_ANGLE_MUL)) * 5;
    }
    else
    {
        // No collision
        return 0;
    }
}