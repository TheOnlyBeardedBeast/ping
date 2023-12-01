#include "Ball.h"
#include "utils.h"

void Ball::setMotors(XYS *_steppers)
{
    this->_steppers = _steppers;
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

    // int a0 = this->_steppers->getX();
    // int b0 = this->_steppers->getY();

    // this->dx = abs(a - a0);
    // this->dy = abs(b - b0);
    


    // // Calculating the angle in radians fo the next relative movement
    // #if (LINE == 0)
    //     double rads = atan(((double)this->dy) / ((double)this->dx));
    // #endif
    
    // Point currentPosition = this->getPosition();
    // // this->lastAngle = atan(double(currentPosition.x - x) / double(currentPosition.y - y));
    // // Separating the movement into its vectors
    // // amodifier^2 + bmodifier^2 = 1^2
    // // amodifier^2 + bmodifier^2 = 1
    // // (amodifier*speed)^2 + (bmodifier*speed)^2 = speed^2
    // #if(LINE == 0)
    //     double amodifier = cos(rads);
    //     double bmodifier = sin(rads);
    // #endif

    this->_steppers->setPosition(a,b);

    // #if (LINE == 1)
    //     this->_stepperA->setMaxSpeed(speed);
    //     this->_stepperA->setAcceleration(ACCELERATION);
    //     this->_stepperB->setMaxSpeed(speed);
    //     this->_stepperB->setAcceleration(ACCELERATION);
    // # else
    //     this->_stepperA->setMaxSpeed(amodifier * speed);
    //     this->_stepperA->setAcceleration(amodifier * ACCELERATION);
    //     this->_stepperB->setMaxSpeed(bmodifier * speed);
    //     this->_stepperB->setAcceleration(bmodifier * ACCELERATION);
    // #endif
}

void Ball::stop()
{
    this->_steppers->stop();
}

Point Ball::getPosition()
{
    // Getting A and B stepper position
    long a = this->_steppers->getX();
    long b = this->_steppers->getY();

    // Creating the result struct and converting motor positions to X and Y coordinated 
    Point result = Point();
    result.x = (a + b) >> 1;
    result.y = (a - b) >> 1;

    return result;
}

void Ball::postCalibrationStop()
{
    this->stop();

    this->waitRun();
    
}

void Ball::waitRun()
{
    while (this->needsToMove());
}

void Ball::calibrate()
{
    
    // #if (DEBUG == 1)
    //         Serial.println("calibration start");
    //     #endif
    // Initialize visited limit switch array
    this->initCalibration();
    
    // Looking for the left edge
    // -10000,0 = -10000,-10000
    this->setposition(-CALIBRATION_LENGTH,0,CALIBRATION_SPEED);
    while (digitalRead(30));
    // {
    //     // #if DEBUG == 1
    //     //     Serial.println("Wait left");
    //     // #endif
    // }

    this->stop();
    // this->postCalibrationStop();
    // delay(1000);


    // Looking for the top edge
    // -1000,-10000 = -11000, 9000
    this->setposition(this->getPosition().x,-CALIBRATION_LENGTH,CALIBRATION_SPEED);
    while (digitalRead(32));
    // {
    //     // #if (DEBUG == 1)
    //     //     Serial.println("Wait top");
    //     // #endif
    // }

    this->stop();
    // this->postCalibrationStop();
    // delay(1000);

    // If we have the left and the bottom edge we can set our origin point
    // this->_stepperA->setCurrentPosition(-SAFEZONE_WIDTH);
    // this->_stepperB->setCurrentPosition(-SAFEZONE_WIDTH);
    this->_steppers->setCurrentPosition(-SAFEZONE_WIDTH,-SAFEZONE_WIDTH);

    // Looking for the bottom edge
    // 0, 10000
    this->setposition(0,CALIBRATION_LENGTH, CALIBRATION_SPEED);
    while (digitalRead(34));
    // {
    //     // #if (DEBUG == 1)
    //     //     Serial.println("Wait bottom");
    //     // #endif
    // }

    this->stop();
    // this->postCalibrationStop();
    // delay(1000);

    Point midCalibrationPosition = this->getPosition();
    this->limits.y = midCalibrationPosition.y - SAFEZONE_WIDTH;

    // this->setposition(0,this->limits.y>>1,CALIBRATION_SPEED);
    this->setposition(midCalibrationPosition.x,this->limits.y>>1,CALIBRATION_SPEED);

    waitRun();

    // Looking for the right edge
    this->setposition(CALIBRATION_LENGTH,this->getPosition().y,CALIBRATION_SPEED);
    while (digitalRead(36));
    // {
    //     // #if (DEBUG == 1)
    //     //     Serial.println("Wait right");
    //     // #endif
        
    // }

    this->stop();
    // this->postCalibrationStop();
    // delay(1000);

    // Setting the max limit for the axes
    Point position = this->getPosition();
    this->limits.x = position.x - SAFEZONE_WIDTH;
    // this->limits.y = position.y - SAFEZONE_WIDTH;
    

    // Center the ball
    this->runCenter();
}

void Ball::initCalibration() 
{
    for (byte i = 0; i < 4; i++)
    {
        this->calibrationState[i] = false;
    }
}

void Ball::runCenter()
{
    this->center();

    this->waitRun();
    
    this->stop();

    this->waitRun();
}

void Ball::center()
{
    this->setposition(this->limits.x >> 1, this->limits.y>>1);
}


bool Ball::needsToMove()
{
    return this->_steppers->isMoving();
}

void Ball::bounce()
{
    // float x = sin(this->lastAngle);
    // float y = cos(this->lastAngle);

    shootAngle(this->lastAngle < 1 ? PI-0.523598776 : 0.523598776);
}

void Ball::shootAngle(double rads)
{
    this->lastAngle = rads;
    Point position = this->getPosition();

    double tanrads = tan(rads);
    bool SHOOT_LEFT = rads > PI*0.5 && rads < PI*1.5;

    double adjacent = SHOOT_LEFT ? -(this->limits.x-position.x) : position.x;
    double opposite = adjacent * tanrads;
    
    this->setposition(SHOOT_LEFT ? this->limits.x : 0,position.y + opposite);
}