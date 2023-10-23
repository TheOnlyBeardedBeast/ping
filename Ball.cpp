#include "Ball.h"
#include "utils.h"
#include "AccelStepper.h"

void Ball::setMotors(AccelStepper *_stepperA, AccelStepper *_stepperB)
{
     this->_stepperA = _stepperA;
    this->_stepperA->setPinsInverted(true);
    this->_stepperB = _stepperB;
    this->_stepperB->setPinsInverted(true);
}

void Ball::run()
{
    // int dx = abs(x1 - x0);
    // int dy = abs(y1 - y0);
    // int sx = x0 < x1 ? 1 : -1;
    // int sy = y0 < y1 ? 1 : -1;
    // int err = dx - dy;

        // Todo here

        if (_stepperA->distanceToGo() == 0 && _stepperA->distanceToGo() == 0) {
            this->_stepperA->run();
            this->_stepperB->run();
        }

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            // x0 += sx;
            this->_stepperA->run();
        }
        if (e2 < dx) {
            err += dx;
            // y0 += sy;
            this->_stepperB->run();
        }
    
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

    int a0 = this->_stepperA->currentPosition();
    int b0 = this->_stepperB->currentPosition();

    this->dx = abs(a - a0);
    this->dy = abs(b - b0);
    this->sx = a0 < a ? 1 : -1;
    this->sy = b0 < b ? 1 : -1;
    this->err = dx - dy;

    // Calculating the angle in radians fo the next relative movement
    double rads = atan(((double)this->dy) / ((double)this->dx));
    
    Point currentPosition = this->getPosition();
    // this->lastAngle = atan(double(currentPosition.x - x) / double(currentPosition.y - y));
    // Separating the movement into its vectors
    // amodifier^2 + bmodifier^2 = 1^2
    // amodifier^2 + bmodifier^2 = 1
    // (amodifier*speed)^2 + (bmodifier*speed)^2 = speed^2
    double amodifier = cos(rads);
    double bmodifier = sin(rads);

    this->_stepperA->moveTo(a);
    this->_stepperA->setMaxSpeed(amodifier * speed);
    this->_stepperA->setAcceleration(amodifier * ACCELERATION);

    this->_stepperB->moveTo(b);
    this->_stepperB->setMaxSpeed(bmodifier * speed);
    this->_stepperB->setAcceleration(bmodifier * ACCELERATION);


    
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

void Ball::postCalibrationStop()
{
    this->stop();

    this->waitRun();
    
}

void Ball::waitRun()
{
    while (this->needsToMove())
    {
        this->run();
    }
}

void Ball::calibrate()
{
    // Initialize visited limit switch array
    this->initCalibration();
    
    // Looking for the left edge
    this->setposition(-CALIBRATION_LENGTH,0,CALIBRATION_SPEED);
    while (digitalRead(22))
    {
        this->run();
    }

    this->stop();
    //this->postCalibrationStop();


    // Looking for the top edge
    this->setposition(this->getPosition().x,-CALIBRATION_LENGTH,CALIBRATION_SPEED);
    while (digitalRead(23))
    {
        this->run();
    }

    this->stop();
    //this->postCalibrationStop();

    // If we have the left and the bottom edge we can set our origin point
    this->_stepperA->setCurrentPosition(-SAFEZONE_WIDTH);
    this->_stepperB->setCurrentPosition(-SAFEZONE_WIDTH);

    // Looking for the bottom edge
    this->setposition(0,CALIBRATION_LENGTH, CALIBRATION_SPEED);
    while (digitalRead(24))
    {
        this->run();
    }

    this->stop();
    //this->postCalibrationStop();

    Point midCalibrationPosition = this->getPosition();
    this->limits.y = midCalibrationPosition.y - SAFEZONE_WIDTH;

    // this->setposition(0,this->limits.y>>1,CALIBRATION_SPEED);
    this->setposition(midCalibrationPosition.x,this->limits.y>>1,CALIBRATION_SPEED);

    waitRun();

    // Looking for the right edge
    this->setposition(CALIBRATION_LENGTH,this->getPosition().y,CALIBRATION_SPEED);
    while (digitalRead(25))
    {
        this->run();
    }

    this->stop();
    //this->postCalibrationStop();

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
    return this->_stepperA->distanceToGo() != 0 && this->_stepperB->distanceToGo() != 0;
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