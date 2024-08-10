#include "Ball.h"
#include "utils.h"

void Ball::setMotors(XYS *_steppers)
{
    this->_steppers = _steppers;
}

void Ball::setposition(int x, int y)
{
    this->setposition(x, y, SPEED);
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
    this->_steppers->setCurrentPosition(2000, 2000);
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
    // float x = sin(this->lastAngle);
    // float y = cos(this->lastAngle);
    // DEBUG
    Serial.println("Bounce call");
    // END
    if (this->lastAngle < PI)
    {
        this->shootAngle((float)PI - this->lastAngle);
    }
    else
    {
        this->shootAngle(((float)(PI * 2)) - this->lastAngle + (float)PI);
    }
    // shootAngle(this->lastAngle < 1 ? PI-0.523598776 : 0.523598776);
}

void Ball::shootAngle(float rads)
{
    this->lastAngle = rads;
    Point position = this->getPosition();

    float tanrads = tan(rads);
    bool SHOOT_LEFT = rads > PI * 0.5 && rads < PI * 1.5;

    double adjacent = SHOOT_LEFT ? -(this->limits.x - position.x) - SAFEZONE_WIDTH : (position.x) - SAFEZONE_WIDTH;
    float opposite = adjacent * tanrads;

    this->setposition(SHOOT_LEFT ? this->limits.x - SAFEZONE_WIDTH : SAFEZONE_WIDTH, position.y + opposite);
    // DEBUG
    Serial.println("Shoot call");
    Serial.print("Angle:");
    Serial.println(rads);
    Serial.print("x:");
    Serial.println(SHOOT_LEFT ? this->limits.x - SAFEZONE_WIDTH : SAFEZONE_WIDTH);
    Serial.print("y:");
    Serial.println(position.y + opposite);
    // END
}

void Ball::stopNow()
{
    this->_steppers->stopNow();
}