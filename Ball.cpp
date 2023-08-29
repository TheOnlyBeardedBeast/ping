#include "Ball.h"

void Ball::run()
{
}

void Ball::setposition(int x, int y)
{
    int a = x + y;
    int b = x - y;

    this->_stepperA->moveTo(a);
    this->_stepperB->moveTo(b);
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
