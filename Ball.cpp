#include "Ball.h"
#include "utils.h"

void Ball::run()
{
}

void Ball::setposition(int x, int y)
{
    int a = x + y;
    int b = x - y;

    double rads = atan((b - this->_stepperB) / (a - _stepperA->currentPosition));
    double amodifier = cos(rads);
    double bmodifier = sin(rads);

    this->_stepperA->moveTo(a);
    this->_stepperA->setMaxSpeed(amodifier * SPEED);

    this->_stepperB->moveTo(b);
    this->_stepperB->setMaxSpeed(bmodifier * SPEED);
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
