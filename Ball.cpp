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
    this->setposition(x,y,SPEED);
}

void Ball::setposition(int x, int y, int speed)
{
    // Calculating absolute A and B motor position from absolute X and Y coordinates
    // Formula used for corexy and h-bot positioning
    int a = x + y;
    int b = x - y;

    // Calculating the angle in radians fo the next relative movement
    double rads = atan(((double)b - (double)this->_stepperB->currentPosition()) / ((double)a - (double)this->_stepperA->currentPosition()));
    
    // Separating the movement into its vectors
    // amodifier^2 + bmodifier^2 = 1^2
    // amodifier^2 + bmodifier^2 = 1
    // (amodifier*speed)^2 + (bmodifier*speed)^2 = speed^2
    double amodifier = cos(rads);
    double bmodifier = sin(rads);

    this->_stepperA->moveTo(a);
    this->_stepperA->setMaxSpeed(amodifier * speed);
    this->_stepperA->setAcceleration(ACCELERATION);

    this->_stepperB->moveTo(b);
    this->_stepperB->setMaxSpeed(bmodifier * speed);
    this->_stepperB->setAcceleration(ACCELERATION);
}

void Ball::stop()
{
    this->_stepperA->stop();
    this->_stepperB->stop();
}

Point Ball::getPosition()
{
    // Getting A and B stepper position
    long a = this->_stepperA->currentPosition();
    long b = this->_stepperB->currentPosition();

    // Creating the result struct and converting motor positions to X and Y coordinated 
    Point result = Point();
    result.x = (a + b) >> 1;
    result.y = (a - b) >> 1;

    return result;
}

void Ball::calibrate()
{
    // Initialize visited limit switch array
    this->initCalibration();
    
    // Looking for the bottom edge
    this->setposition(-10000,0,CALIBRATION_SPEED);
    while (!this->calibrationState[0])
    {
        this->run();
    }

    this->stop();


    // Looking for the left edge
    this->setposition(this->getPosition().x,-10000,CALIBRATION_SPEED);
    while (!this->calibrationState[1])
    {
        this->run();
    }

    this->stop();

    // If we have the left and the bottom edge we can set our origin point
    this->_stepperA->setCurrentPosition(-SAFEZONE_WIDTH);
    this->_stepperB->setCurrentPosition(-SAFEZONE_WIDTH);

    // Looking for the right edge
    this->setposition(0,10000, CALIBRATION_SPEED);
    while (!this->calibrationState[2])
    {
        this->run();
    }

    this->stop();


    // Looking for the top edge
    this->setposition(10000,this->getPosition().y,CALIBRATION_SPEED);
    while (!this->calibrationState[3])
    {
        this->run();
    }

    this->stop();

    // Setting the max limit for the axes
    Point position = this->getPosition();
    this->limits.x = position.x - SAFEZONE_WIDTH;
    this->limits.y = position.y - SAFEZONE_WIDTH;

    // Center the ball
    this->center();
}

void Ball::initCalibration() 
{
    for (byte i = 0; i < 4; i++)
    {
        this->calibrationState[i] = false;
    }
}

void Ball::center()
{
    this->setposition(this->limits.x >> 1, this->limits.y>>1);

    while (this->_stepperA->distanceToGo() != 0 || this->_stepperB->distanceToGo() != 0)
    {
        this->run();
    }
    
    this->stop();
}


bool Ball::needsToMove()
{
    return this->_stepperA->distanceToGo() != 0 && this->_stepperB->distanceToGo() != 0;
}