#include "Ball.h"
#include "utils.h"
#include "AccelStepper.h"

void Ball::setMotors(AccelStepper *_stepperA, AccelStepper *_stepperB)
{
    this->_stepperA = _stepperA;
    this->_stepperB = _stepperB;
}

void Ball::run()
{
    this->_stepperA->run();
    this->_stepperB->run();
}

void Ball::setposition(int x, int y)
{
    int a = x + y;
    int b = x - y;

    double rads = atan(((double)b - (double)this->_stepperB->currentPosition()) / ((double)a - (double)this->_stepperA->currentPosition()));
    
    double amodifier = cos(rads);
    double bmodifier = sin(rads);

    this->_stepperA->moveTo(a);
    this->_stepperA->setMaxSpeed(amodifier * SPEED);
    this->_stepperA->setAcceleration(ACCELERATION);

    this->_stepperB->moveTo(b);
    this->_stepperB->setMaxSpeed(bmodifier * SPEED);
    this->_stepperB->setAcceleration(ACCELERATION);
}

void Ball::stop()
{
    this->_stepperA->stop();
    this->_stepperB->stop();
}

Point Ball::getPosition()
{
    long a = this->_stepperA->currentPosition();
    long b = this->_stepperB->currentPosition();

    Point result = Point();
    result.x = (a + b) >> 1;
    result.y = (a - b) >> 1;

    return result;
}
