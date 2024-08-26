#include "Ball.h"
#include "utils.h"

Ball *Ball::instance;

void Ball::setMotors(XYS *_steppers)
{
    this->_steppers = _steppers;
}

void Ball::setposition(int x, int y)
{
    this->setposition(x, y, SPEED);
}

void Ball::setCurrentPosition(int x, int y)
{
    int a = x + y;
    int b = x - y;

    this->_steppers->setCurrentPosition(a, b);
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

    this->_steppers->setPosition(a, b);

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

long Ball::getCenterRelativePosition()
{
    return this->getPosition().y - (this->limits.y >> 1);
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
        delay(1);
    }
}

void Ball::calibrate()
{
    // Serial.println("Calibration methiod");
    // #if (DEBUG == 1)
    //         Serial.println("calibration start");
    //     #endif
    // Initialize visited limit switch array
    this->initCalibration();

    // Looking for the left edge
    // -10000,0 = -10000,-10000
    // this->setposition(-CALIBRATION_LENGTH,0,CALIBRATION_SPEED);

    BoolCallback leftLimitHit = []()
    { return digitalRead(LS1) == HIGH ? true : false; };

    this->_steppers->moveWhile(HIGH, HIGH, CALIBRATION_SPEED, leftLimitHit);
    delay(200);
    this->setCurrentPosition(2400, 2260);
    this->limits.x = 2400;
    this->limits.y = 2260; // 1step = 0.025cm // somehow 2280 works better maybe belt tension issue
    delay(200);
    return;
    // while (digitalRead(30));
    // {
    //     // #if DEBUG == 1
    //     //     Serial.println("Wait left");
    //     // #endif
    // }

    // this->stop();
    // this->postCalibrationStop();
    // delay(1000);

    // Looking for the top edge
    // -1000,-10000 = -11000, 9000
    BoolCallback topLimitHit = []()
    { return digitalRead(LS2) == HIGH ? true : false; };

    this->_steppers->moveWhile(LOW, HIGH, CALIBRATION_SPEED, topLimitHit);
    delay(200);
    // this->setposition(this->getPosition().x,-CALIBRATION_LENGTH,CALIBRATION_SPEED);
    // while (digitalRead(32));
    // {
    //     // #if (DEBUG == 1)
    //     //     Serial.println("Wait top");
    //     // #endif
    // }

    // this->stop();
    // this->postCalibrationStop();
    // delay(1000);

    // If we have the left and the bottom edge we can set our origin point
    // this->_stepperA->setCurrentPosition(-SAFEZONE_WIDTH);
    // this->_stepperB->setCurrentPosition(-SAFEZONE_WIDTH);

    // // DEBUG
    // Point debugPoint =  this->getPosition();
    // Serial.println("Before low limit set");
    // Serial.print("x:");
    // Serial.println(debugPoint.x);
    // Serial.print("y:");
    // Serial.println(debugPoint.y);
    // // END

    this->_steppers->setCurrentPosition(0, 0);

    // // DEBUG
    // debugPoint =  this->getPosition();
    // Serial.println("After low limit set");
    // Serial.print("x:");
    // Serial.println(debugPoint.x);
    // Serial.print("y:");
    // Serial.println(debugPoint.y);
    // // END

    // Looking for the bottom edge
    // 0, 10000
    BoolCallback bottomLimitHit = []()
    { return digitalRead(LS3) == HIGH ? true : false; };

    this->_steppers->moveWhile(HIGH, LOW, CALIBRATION_SPEED, bottomLimitHit);
    delay(200);
    // this->setposition(0,CALIBRATION_LENGTH, CALIBRATION_SPEED);
    // while (digitalRead(34));
    // {
    //     // #if (DEBUG == 1)
    //     //     Serial.println("Wait bottom");
    //     // #endif
    // }

    // this->stop();
    // this->postCalibrationStop();
    // delay(1000);

    Point midCalibrationPosition = this->getPosition();
    this->limits.y = midCalibrationPosition.y;

#if defined(ARDUINO_GIGA)
    digitalWrite(LEDB, HIGH);
#elif defined(ARDUINO_SAM_DUE)
    digitalWrite(LED_BUILTIN, HIGH);
#endif
    this->setposition(0, this->limits.y >> 1, CALIBRATION_SPEED);
    // this->setposition(midCalibrationPosition.x,this->limits.y>>1,CALIBRATION_SPEED);

    waitRun();
#if defined(ARDUINO_GIGA)
    digitalWrite(LEDB, LOW);
#elif defined(ARDUINO_SAM_DUE)
    digitalWrite(LED_BUILTIN, LOW);
#endif

    // Looking for the right edge
    // this->setposition(CALIBRATION_LENGTH,this->getPosition().y,CALIBRATION_SPEED);
    // while (digitalRead(36));
    BoolCallback rightLimitHit = []()
    { return digitalRead(LS4) == HIGH ? true : false; };

    this->_steppers->moveWhile(HIGH, HIGH, CALIBRATION_SPEED, rightLimitHit);
    delay(200);
    // {
    //     // #if (DEBUG == 1)
    //     //     Serial.println("Wait right");
    //     // #endif

    // }

    // this->stop();
    // this->postCalibrationStop();
    // delay(1000);

    // Setting the max limit for the axes
    Point position = this->getPosition();
    this->limits.x = position.x;
    // this->limits.y = position.y - SAFEZONE_WIDTH;

    // // DEBUG
    // Serial.println("After high limit set");
    // Serial.print("x:");
    // Serial.println(this->limits.x);
    // Serial.print("y:");
    // Serial.println(this->limits.y);
    // // END

    // Center the ball
    this->runCenter();

    delay(5000);
#if defined(ARDUINO_GIGA)
    digitalWrite(LEDB, HIGH);
#elif defined(ARDUINO_SAM_DUE)
    digitalWrite(LED_BUILTIN, HIGH);
#endif
}

void Ball::initCalibration()
{
    this->calibrationState[0] = false;
    this->calibrationState[1] = false;
    this->_steppers->setCurrentPosition(0, 0);
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
    this->setposition(this->limits.x >> 1, this->limits.y >> 1);
}

bool Ball::needsToMove()
{
    return this->_steppers->isMoving();
}

void Ball::bounce()
{
    if (this->lastAngle < 180)
    {
        this->shootDeg(180 - this->lastAngle);
    }
    else
    {
        this->shootDeg(540 - this->lastAngle);
    }
}

void Ball::shootDeg(uint16_t degrees)
{
    this->lastAngle = degrees;
    this->shootAngle((float)degrees * (float)PI / 180.f);
}

void Ball::shootAngle(float angleRadians)
{
    Point ballPos = this->getPosition();

    if ((ballPos.y == this->limits.y &&
         (this->lastAngle < 90 || this->lastAngle > 270)) ||
        (ballPos.y == 0 &&
         (this->lastAngle > 90 && this->lastAngle < 270)))
    {
        return this->shootDeg(this->inverseAngle(this->lastAngle));
    }

    double dy = cos(angleRadians);
    double dx = sin(angleRadians);

    bool horizontalModifier = dy >= 0;
    bool verticalModifier = dx >= 0;

    double a = horizontalModifier ? this->limits.y - ballPos.y : ballPos.y;
    double o = verticalModifier ? this->limits.x - ballPos.x : ballPos.x;

    double ty = a / dy;
    double tx = o / dx;

    double t = min(abs(ty), abs(tx));

    int newX = constrain(ballPos.x + round(dx * t), 0, this->limits.x);
    int newY = constrain(ballPos.y + round(dy * t), 0, this->limits.y);

    this->setposition(newX, newY);
}

uint16_t Ball::inverseAngle(int16_t angle)
{
    if (angle < 180)
    {
        return 180 - angle;
    }

    return 540 - angle;
}

void Ball::stopNow()
{
    this->_steppers->stopNow();
}