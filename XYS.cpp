#include "XYS.h"
#include "GigaDigitalWriteFast.h"

#define SPEED 400
#define ACCELERATION 10*4000
#define TICKS 1000000

void XYS::setCallback(VoidCallback callback)
{
    this->callback = callback;
}

#if defined(ARDUINO_GIGA)
void XYS::setTimer(Portenta_H7_Timer *timer)
{
    this->timer = timer;
};
#endif
#if defined(ARDUINO_SAM_DUE)
void XYS::setTimer(DueTimer *timer)
{
    this->timer = timer;
};
#endif


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
        #if defined(ARDUINO_GIGA)
            this->timer->stopTimer();
            this->timer->detachInterrupt();
        #endif
        #if defined(ARDUINO_SAM_DUE)
            this->timer->stop();
            this->timer->detachInterrupt();
        #endif

        this->resetBresenham();
        this->resetRamping();
        this->moving = false;
        
        return;
    }

    uint16_t stepMask;

    int e2 = 2 * err;
    if (e2 > -dy)
    {
        err -= dy;
        this->x += sx;
        digitalWriteFast(this->stepperX.dir_pin, this->sx > 0 ? HIGH : LOW);
        stepMask |= stepperX.step_mask;

        // digitalWrite(this->stepperX.dir_pin, this->sx > 0 ? HIGH : LOW);
        // digitalWrite(this->stepperX.step_pin, HIGH);
        // delayMicroseconds(1);
        // digitalWrite(this->stepperX.step_pin, LOW);
    }
    if (e2 < dx)
    {
        err += dx;
        this->y += sy;
        digitalWriteFast(this->stepperY.dir_pin, this->sy > 0 ? HIGH : LOW);
        stepMask |= stepperY.step_mask;

        // digitalWrite(this->stepperY.dir_pin, this->sy > 0);
        // digitalWrite(this->stepperY.step_pin, HIGH);
        // delayMicroseconds(1);
        // digitalWrite(this->stepperY.step_pin, LOW);
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
    } else {
        // noo need for any delay recalculation
        return;
    }

    #if defined(ARDUINO_GIGA)
    this->timer->setInterval(this->delayPeriod-3, this->callback);
    this->speed = TICKS / this->delayPeriod;
    #endif
    #if defined(ARDUINO_SAM_DUE)
    this->timer->stop();
    this->timer->setPeriod(this->delayPeriod).start();
    #endif
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

    // Leib ramp
    this->distance = abs(this->dx > this->dy ? this->dx : this->dy);
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

    // if(this->axis == MainAxis::X)
    // {
        this->distance = this->distanceRun + this->deccelDistance;
    // }
};

bool XYS::needsMoving()
{
    return this->distanceRun != this->distance;
};

void XYS::startTimer(float frequency)
{
    #if defined(ARDUINO_GIGA)
    this->timer->attachInterruptInterval(this->delayPeriod, this->callback);
    #endif
    #if defined(ARDUINO_SAM_DUE)
    this->timer->attachInterrupt(this->callback).setPeriod(this->delayPeriod).start();
    #endif
    
};

void XYS::stepLeft()
{
    uint16_t stepMask = stepperX.step_mask | stepperY.step_mask;

    digitalWriteFast(stepperX.dir_pin,LOW);
    digitalWriteFast(stepperY.dir_pin,LOW);
    
    digitalToggleMask(stepMask,this->port);
    delayMicroseconds(3);
    digitalToggleMask(stepMask,this->port);
    this->x--;
}

void XYS::stepRight()
{
    uint16_t stepMask = stepperX.step_mask | stepperY.step_mask;

    digitalWriteFast(stepperX.dir_pin,HIGH);
    digitalWriteFast(stepperY.dir_pin,HIGH);
    
    digitalToggleMask(stepMask,this->port);
    delayMicroseconds(3);
    digitalToggleMask(stepMask,this->port);
    this->x++;
}

