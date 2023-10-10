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

void Ball::runSpeed()
{
    this->_stepperA->runSpeed();
    this->_stepperB->runSpeed();
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
    this->_stepperA->setSpeed(amodifier * CALIBRATION_SPEED);
    this->_stepperA->setAcceleration(ACCELERATION);

    this->_stepperB->moveTo(b);
    this->_stepperB->setMaxSpeed(bmodifier * SPEED);
    this->_stepperB->setSpeed(bmodifier * CALIBRATION_SPEED);
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

void Ball::calibrate()
{
    
    
    for (byte i = 0; i < 4; i++)
    {
        this->calibrationState[i] = false;
    }
    
    this->setposition(0,-100);
    while (!this->calibrationState[0])
    {
        Serial.print(this->_stepperA->speed());
        Serial.print(this->_stepperB->speed());
        Serial.println("Go for y calibration");
        this->runSpeed();
    }

    Serial.println(this->_stepperA->currentPosition());
    Serial.println(this->_stepperB->currentPosition());

    this->setposition(-100,this->getPosition().y);
    while (!this->calibrationState[1])
    {
        Serial.print(this->_stepperA->speed());
        Serial.print(this->_stepperB->speed());
        Serial.println("Go for x calibration");
        this->runSpeed();
    }

    Serial.println(this->_stepperA->currentPosition());
    Serial.println(this->_stepperB->currentPosition());

    Serial.println("calibrated");


    // this->setposition(0,10000);
    // while (!this->calibrationState[1])
    // {
    //     this->runSpeed();
    // }

    

    // this->setposition(10000,0);
    // while (!this->calibrationState[3])
    // {
    //     this->runSpeed();
    // }
    
}
