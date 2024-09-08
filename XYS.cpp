#include "XYS.h"
#if defined(ARDUINO_GIGA)
#include "GigaDigitalWriteFast.h"
#elif defined(ARDUINO_SAM_DUE)
#include "DueWriteFast.h"
#endif
#include "helpers.h"

#include "ClearTarget.h"

extern ClearTarget clearTimes[4];

#define SPEED 1800
#define ACCELERATION 10000
#define ACC2 20000
#define TICKS 1000000
#define START_SPEED 200
#define END_SPEED 200

#define TRAPEZOID 1

#if defined(ARDUINO_GIGA)
void XYS::setTimer(Portenta_H7_Timer *timer)
{
    this->timer = timer;
};
#elif defined(ARDUINO_SAM_DUE)
void XYS::setTimer(DueTimer *timer)
{
    this->timer = timer;
};
#endif

XYS *XYS::instance;

void XYS::init(int stepX, int dirX, int stepY, int dirY)
{
    GPIO_TypeDef *stepXPort = getPinPort(stepX);
    GPIO_TypeDef *stepYPort = getPinPort(stepY);

    // if(stepXPort != stepYPort)
    // {
    //     throw "Pins are not on a the same port.";
    // }

    this->stepperX.step_pin = stepX;
    this->stepperX.dir_pin = dirX;
    this->stepperX.step_mask = getPinMask(stepX);
    // PB_7
    // PB_7
    // GPIOB->ODR // output set
    // GPIOB->IDR // input read

    this->stepperY.step_pin = stepY;
    this->stepperY.dir_pin = dirY;
    this->stepperY.step_mask = getPinMask(stepY);

    this->port = stepXPort;

    XYS::instance = this;
};

#if TRAPEZOID == 0
void XYS::step()
{
    // int currentTime = micros();

    // if(currentTime-lastStepAt < this->delayPeriod)
    // {
    //     return;
    // }

    if (!this->needsMoving())
    {
#if defined(ARDUINO_GIGA)
        this->timer->stopTimer();
#elif defined(ARDUINO_SAM_DUE)
        this->timer->stop();
#endif

        // this->timer->detachInterrupt();

        // this->clearStep();
        // this->cleared = true;
        this->resetBresenham();
        this->resetRamping();
        this->moving = false;

        return;
    }

    // if (!this->cleared)
    // {
    //     this->clearStep();
    //     this->cleared = !this->cleared;
    //     return;
    // }

    // this->cleared = !this->cleared;

    uint16_t stepMask = 0;

    int e2 = this->err << 1;
    if (e2 > -dy)
    {
        err -= dy;
        this->x += sx;
        stepMask |= stepperX.step_mask;
    }
    if (e2 < dx)
    {
        err += dx;
        this->y += sy;
        stepMask |= stepperY.step_mask;
    }

    digitalToggleMask(stepMask, this->port);
    delayMicroseconds(3);
    digitalToggleMask(stepMask, this->port);

    this->distanceRun++;

    float Q = this->multiplier * this->delayPeriod * this->delayPeriod;

    if (this->distanceRun < this->accelDistance)
    {
        // this->delayPeriod = this->delayPeriod * (1-this->multiplier*this->delayPeriod*this->delayPeriod);
        this->delayPeriod = this->delayPeriod * (1 - Q + Q * Q);
    }
    else if (this->distanceRun >= this->distance - this->deccelDistance)
    {
        // this->delayPeriod = this->delayPeriod * (1+this->multiplier*this->delayPeriod*this->delayPeriod);
        this->delayPeriod = this->delayPeriod * (1 + Q + Q * Q);
    }

#if defined(ARDUINO_GIGA)
    this->timer->setInterval(this->delayPeriod, XYS::ballIsr);
#elif defined(ARDUINO_SAM_DUE)
    this->timer->setPeriod(this->delayPeriod).start();
#endif

    this->speed = TICKS / this->delayPeriod;
};
#endif

#if TRAPEZOID == 1
void XYS::step()
{
    if (!this->needsMoving())
    {
#if defined(ARDUINO_GIGA)
        this->timer->stopTimer();
#elif defined(ARDUINO_SAM_DUE)
        this->timer->stop();
#endif

        // this->timer->detachInterrupt();

        this->resetBresenham();
        this->resetRamping();
        this->moving = false;

        return;
    }

    uint16_t stepMask = 0;

    int e2 = this->err << 1;
    if (e2 > -dy)
    {
        err -= dy;
        this->x += sx;
        stepMask |= stepperX.step_mask;
    }
    if (e2 < dx)
    {
        err += dx;
        this->y += sy;
        stepMask |= stepperY.step_mask;
    }

    digitalToggleMask(stepMask, this->port);
    delayMicroseconds(3);
    digitalToggleMask(stepMask, this->port);

    this->distanceRun++;

    if (this->distanceRun < this->accelDistance)
    {
        // this->delayPeriod = this->delayPeriod * (1-this->multiplier*this->delayPeriod*this->delayPeriod);
        this->speed = START_SPEED + sqrt(ACC2 * this->distanceRun + START_SPEED * START_SPEED);
    }
    else if (this->distanceRun > this->distance - this->deccelDistance)
    {
        int decelStepIndex = this->distanceRun - (this->distance - this->deccelDistance);
        // this->delayPeriod = this->delayPeriod * (1+this->multiplier*this->delayPeriod*this->delayPeriod);
        this->speed = END_SPEED + sqrt(this->targetSpeed * this->targetSpeed - ACC2 * decelStepIndex);
    }
    else
    {
        this->speed = this->targetSpeed;
    }

    this->delayPeriod = TICKS / constrain(this->speed, min(START_SPEED, END_SPEED), this->targetSpeed);

#if defined(ARDUINO_GIGA)
    this->timer->setInterval(this->delayPeriod - 3, XYS::ballIsr);
#elif defined(ARDUINO_SAM_DUE)
    this->timer->setPeriod(this->delayPeriod - 3).start();
#endif
};

#endif

#if TRAPEZOID == 0
void XYS::setPosition(long x, long y, int moveSpeed = SPEED)
{
    this->moving = true;
    // this->targetX = x;
    // this->targetY = y;

    // Bresenham
    this->dx = abs(x - this->x);
    this->dy = abs(y - this->y);
    this->sx = this->x < x ? 1 : -1;
    this->sy = this->y < y ? 1 : -1;
    this->err = this->dx - this->dy;
    this->axis = this->dx < this->dy ? MainAxis::Y : MainAxis::X;

    // DirectionSetup
    // It is enough to setup the direction just here, because it wont change in the next steps
    digitalWriteFast(this->stepperX.dir_pin, this->sx > 0 ? HIGH : LOW);
    digitalWriteFast(this->stepperY.dir_pin, this->sy > 0 ? HIGH : LOW);

    // RAMPING
    this->distance = abs(this->dx >= this->dy ? this->dx : this->dy);
    this->distanceRun = 0;
    int halfDistance = this->distance >> 1;

    // LEIB RAMP
    float acc2 = (float)(ACCELERATION << 1);
    float accDistance = (float)((moveSpeed * moveSpeed) / acc2);
    if (this->speed == 0)
    {
        this->accelDistance = accDistance;
        // this->deccelDistance = accDistance;

        if (this->accelDistance >= halfDistance)
        {
            this->accelDistance = halfDistance;
            this->deccelDistance = halfDistance;
        }
        else
        {
            this->deccelDistance = this->accelDistance;
        }
    }
    else
    {
        int speedPow = this->speed * this->speed;
        this->accelDistance = moveSpeed * moveSpeed - speedPow / ACC2;
        this->deccelDistance = accDistance;

        if (this->deccelDistance + this->accelDistance >= this->distance)
        {
            if (this->accelDistance < halfDistance)
            {
                this->accelDistance = this->accelDistance;
                this->deccelDistance = this->distance - this->accelDistance;
            }
            else
            {
                this->accelDistance = halfDistance;
                this->deccelDistance = halfDistance;
            }
        }
    }

    this->multiplier = (float)(ACCELERATION) / (float)((float)TICKS * (float)TICKS);
    this->speed = sqrtf(acc2);
    // original TICKS/sqrtf(acc2)
    this->delayPeriod = TICKS / this->speed;
    this->startTimer(this->delayPeriod);
}
#endif

#if TRAPEZOID == 1
void XYS::setPosition(long x, long y, int moveSpeed = SPEED)
{
    this->moving = true;
    this->targetSpeed = (long)moveSpeed;
    // this->targetX = x;
    // this->targetY = y;

    // Bresenham
    this->dx = abs(x - this->x);
    this->dy = abs(y - this->y);
    this->sx = this->x < x ? 1 : -1;
    this->sy = this->y < y ? 1 : -1;
    this->err = this->dx - this->dy;
    this->axis = this->dx < this->dy ? MainAxis::Y : MainAxis::X;

    // DirectionSetup
    // It is enough to setup the direction just here, because it wont change in the next steps
    digitalWriteFast(this->stepperX.dir_pin, this->sx > 0 ? HIGH : LOW);
    digitalWriteFast(this->stepperY.dir_pin, this->sy > 0 ? HIGH : LOW);

    // RAMPING
    this->distance = abs(this->dx >= this->dy ? this->dx : this->dy);
    this->distanceRun = 0;
    int halfDistance = this->distance >> 1;

    // TRAPEZOIDAL RAMP
    int maxSpeedPow = moveSpeed * moveSpeed;

    this->accelDistance = (maxSpeedPow - START_SPEED * START_SPEED) / ACC2;
    this->deccelDistance = (maxSpeedPow - END_SPEED * END_SPEED) / ACC2;
    long totalAccelDecelSteps = this->accelDistance + this->deccelDistance;

    if (this->distance < totalAccelDecelSteps)
    {
        this->accelDistance = this->distance * this->accelDistance / (this->accelDistance + this->deccelDistance);
        this->deccelDistance = this->distance - this->accelDistance;
    }

    this->speed = START_SPEED; // sqrt(acc2*step+startSpeed*startSpeed);
    this->delayPeriod = TICKS / this->speed;
    this->startTimer(this->delayPeriod);
}
#endif

void XYS::stop()
{
    int speedPow = this->speed * this->speed;

    this->accelDistance = SPEED * SPEED - speedPow / (ACCELERATION << 1);
    this->deccelDistance = this->accelDistance;

    this->distance = this->distanceRun + this->deccelDistance;
};

void XYS::stopNow()
{
#if defined(ARDUINO_GIGA)
    this->timer->stopTimer();
#elif defined(ARDUINO_SAM_DUE)
    this->timer->stop();
#endif

    this->timer->detachInterrupt();

    this->resetBresenham();
    this->resetRamping();
    this->moving = false;

    return;
}

bool XYS::needsMoving()
{
    return this->distanceRun != this->distance;
};

void XYS::startTimer(float frequency)
{
#if defined(ARDUINO_GIGA)
    this->timer->attachInterruptInterval(this->delayPeriod, XYS::ballIsr);
#elif defined(ARDUINO_SAM_DUE)
    this->timer->attachInterrupt(XYS::ballIsr).setPeriod(this->delayPeriod).start();
#endif
}
void XYS::setDirection(StepDirection _direction)
{
    if (this->direction == _direction)
    {
        return;
    }

    this->direction = _direction;
    digitalWriteFast(this->stepperX.dir_pin, _direction > 0 ? HIGH : LOW);
    digitalWriteFast(this->stepperY.dir_pin, _direction < 0 ? HIGH : LOW);

    clearTimes[5].enabled = true;
    clearTimes[5].time = micros();
};

void XYS::singleStep()
{
    if (clearTimes[5].enabled || clearTimes[4].enabled)
    {
        return;
    }

    uint16_t stepMask = stepperX.step_mask | stepperY.step_mask;

    digitalWriteFast(stepperX.step_pin, HIGH);
    digitalWriteFast(stepperY.step_pin, HIGH);
    clearTimes[4].enabled = true;
    clearTimes[4].time = micros();
}

void XYS::clearStep()
{
    // uint16_t stepMask = stepperX.step_mask | stepperY.step_mask;
    digitalWriteFast(stepperX.step_pin, LOW);
    digitalWriteFast(stepperY.step_pin, LOW);
}

void XYS::ballIsr()
{
    XYS::instance->step();
}

void XYS::moveWhile(PinStatus motor1, PinStatus motor2, unsigned short speed, BoolCallback condition)
{
    PinStatus xDir = motor1;
    PinStatus yDir = motor2;

    digitalWriteFast(stepperX.dir_pin, motor1);
    digitalWriteFast(stepperY.dir_pin, motor2);

    delayMicroseconds(5);

    int timeDelay = TICKS / speed;

    while (condition())
    {
        digitalWriteFast(stepperX.step_pin, HIGH);
        digitalWriteFast(stepperY.step_pin, HIGH);

        delayMicroseconds(3);

        digitalWriteFast(stepperX.step_pin, LOW);
        digitalWriteFast(stepperY.step_pin, LOW);
        if (xDir && yDir)
        {
            this->x++;
        }
        else if (!xDir && !yDir)
        {
            this->x--;
        }
        if (yDir && !yDir)
        {
            this->y++;
        }
        else if (!yDir && yDir)
        {
            this->y--;
        }

        delayMicroseconds(timeDelay);
    }
}
