#include "XYS.h"
#include "GigaDigitalWriteFast.h"
#include "helpers.h"

#define SPEED 3000
#define ACCELERATION 10*3000
#define TICKS 1000000


void XYS::setTimer(Portenta_H7_Timer *timer)
{
    this->timer = timer;
};

XYS* XYS::instance;

void XYS::init(int stepX, int dirX, int stepY, int dirY)
{
    GPIO_TypeDef* stepXPort = getPinPort(stepX);
    GPIO_TypeDef* stepYPort = getPinPort(stepY);

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

void XYS::step()
{
    // int currentTime = micros();

    // if(currentTime-lastStepAt < this->delayPeriod)
    // {
    //     return;
    // }

    if (!this->needsMoving())
    {

        this->timer->stopTimer();
        this->timer->detachInterrupt();

        this->resetBresenham();
        this->resetRamping();
        this->moving = false;
        
        return;
    }

    uint16_t stepMask = 0;

    int e2 = this->err<<1;
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

    digitalToggleMask(stepMask,this->port);
    delayMicroseconds(3);
    digitalToggleMask(stepMask,this->port);

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
    else {
        return;
    }

    this->timer->setInterval(this->delayPeriod-3, XYS::ballIsr);
    this->speed = TICKS / this->delayPeriod;
    
};

void XYS::setPosition(long x, long y)
{
    this->moving = true;
    // this->targetX = x;
    // this->targetY = y;

    // Bresenham
    this->dx = abs(x - this->x);
    this->dy = abs(y - this->y);
    this->sx = this->x < x ? 1 : -1;
    this->sy = this->y < y ? 1 : -1;
    this->err = dx - dy;
    this->axis = this->dx < this->dy ? MainAxis::Y : MainAxis::X; 

    // DirectionSetup
    // It is enough to setup the direction just here, because it wont change in the next steps
    digitalWriteFast(this->stepperX.dir_pin, this->sx > 0 ? PinStatus::HIGH : PinStatus::LOW);
    digitalWriteFast(this->stepperY.dir_pin, this->sy > 0 ? PinStatus::HIGH : PinStatus::LOW);

    // Leib ramp
    this->distance = abs(this->dx >= this->dy ? this->dx : this->dy);
    this->distanceRun = 0;
    int halfDistance = this->distance >> 1;

    float acc2 = (float)(ACCELERATION << 1);
    float accDistance = (float)((SPEED * SPEED) / acc2);
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
        this->accelDistance = SPEED * SPEED - speedPow / ACCELERATION << 2;
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

void XYS::stop(){
    int speedPow = this->speed * this->speed;

    this->accelDistance = SPEED * SPEED - speedPow / (ACCELERATION<<1);
    this->deccelDistance = this->accelDistance;

    this->distance = this->distanceRun + this->deccelDistance;
};

bool XYS::needsMoving()
{
    return this->distanceRun != this->distance;
};

void XYS::startTimer(float frequency)
{
    this->timer->attachInterruptInterval(this->delayPeriod, XYS::ballIsr);
};


void XYS::stepLeft()
{
    uint16_t stepMask = stepperX.step_mask | stepperY.step_mask;

    // digitalToggleMask(stepMask,this->port);
    digitalWriteFast(stepperX.step_pin,PinStatus::HIGH);
    delayMicroseconds(3);
    digitalWriteFast(stepperX.step_pin,PinStatus::LOW);
    // digitalToggleMask(stepMask,this->port);
    this->x--;
}

void XYS::stepRight()
{
    uint16_t stepMask = stepperX.step_mask | stepperY.step_mask;
    
    digitalToggleMask(stepMask,this->port);
    delayMicroseconds(3);
    digitalToggleMask(stepMask,this->port);
    this->x++;
}

void XYS::ballIsr()
{
    XYS::instance->step();
}

void XYS::moveWhile(PinStatus motor1,PinStatus motor2, unsigned short speed, BoolCallback condition)
{
    PinStatus xDir = motor1;
    PinStatus yDir = motor2;

    digitalWriteFast(stepperX.dir_pin,motor1);
    digitalWriteFast(stepperY.dir_pin,motor2);

    delayMicroseconds(5);

    int timeDelay = TICKS/speed;

    while (condition())
    {
        digitalWriteFast(stepperX.step_pin,PinStatus::HIGH);
        digitalWriteFast(stepperY.step_pin,PinStatus::HIGH);

        delayMicroseconds(3);

        digitalWriteFast(stepperX.step_pin,PinStatus::LOW);
        digitalWriteFast(stepperY.step_pin,PinStatus::LOW);
        if(xDir == HIGH)
        {
            this->x++;
        } else {
            this->x--;
        }
        if(yDir == HIGH)
        {
            this->y++;
        } else {
            this->y--;
        }

        delayMicroseconds(timeDelay);
    }
    
}

