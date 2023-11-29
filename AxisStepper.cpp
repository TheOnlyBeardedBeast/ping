#include "AxisStepper.h"
#include "GigaDigitalWriteFast.h"

#define SPEED 12000
#define ACCELERATION 4*12000
#define TICKS 1000000

void AxisStepper::setCallback(VoidCallback callback)
{
    this->callback = callback;
}

#if defined(ARDUINO_GIGA)
void AxisStepper::setTimer(Portenta_H7_Timer *timer)
{
    this->timer = timer;
};
#endif
#if defined(ARDUINO_SAM_DUE)
void AxisStepper::setTimer(DueTimer *timer)
{
    this->timer = timer;
};
#endif


void AxisStepper::init(int step, int dir)
{
    pinMode(step,OUTPUT);
    this->stepper.step_pin = step;
    pinMode(dir,OUTPUT);
    this->stepper.dir_pin = dir;
    // PB_7
    // GPIOB->ODR // output set
    // GPIOB->IDR // input read
};

void AxisStepper::singleStep(StepDirection direction)
{
    digitalWriteFast(this->stepper.dir_pin, direction > 0 ? HIGH : LOW);
    digitalWriteFast(this->stepper.step_pin, HIGH);
    delayMicroseconds(3);
    digitalWriteFast(this->stepper.step_pin, LOW);
}

void AxisStepper::step()
{
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

        this->resetRamping();
        this->_isRunning = false;
        
        return;
    }

    digitalWriteFast(this->stepper.dir_pin, this->direction > 0 ? HIGH : LOW);
    digitalWriteFast(this->stepper.step_pin, HIGH);
    delayMicroseconds(3);
    digitalWriteFast(this->stepper.step_pin, LOW);
    
    this->position++;
    this->distanceRun++;

    float Q = this->multiplier * this->delayPeriod * this->delayPeriod;

    if (this->distanceRun < this->accelDistance)
    {
        this->delayPeriod = this->delayPeriod * (1 - Q + Q * Q);
    }
    else if (this->distanceRun >= this->distance - this->deccelDistance)
    {
        this->delayPeriod = this->delayPeriod * (1 + Q + Q * Q);
    } else 
    {
        return;
    }

    #if defined(ARDUINO_GIGA)
    this->timer->setInterval(this->delayPeriod, this->callback);
    this->speed = TICKS / this->delayPeriod;
    #endif
    #if defined(ARDUINO_SAM_DUE)
    this->timer->stop();
    this->timer->setPeriod(this->delayPeriod).start();
    #endif
};

void AxisStepper::setPosition(long newDistance)
{
    this->_isRunning = true;
    // Leib ramp
    this->distance = this->distance - newDistance;
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

void AxisStepper::stop(){
    int speedPow = this->speed * this->speed;

    this->accelDistance = SPEED * SPEED - speedPow / (ACCELERATION<<1);
    this->deccelDistance = this->accelDistance;

    this->distance = this->distanceRun + this->deccelDistance;
    
}
void AxisStepper::forceStop(){
    #if defined(ARDUINO_GIGA)
            this->timer->stopTimer();
            this->timer->detachInterrupt();
    #endif
    #if defined(ARDUINO_SAM_DUE)
        this->timer->stop();
        this->timer->detachInterrupt();
    #endif

    this->resetRamping();
}
void AxisStepper::setSpeed(long speed){
    if(!this->isRunning())
    {
        return;
    }
    // Leib ramp
    // this->distance = this->distance - newDistance;
    // this->distanceRun = 0;
    // int halfDistance = this->distance >> 1;

    // float acc2 = (float)(ACCELERATION << 1);
    // float accDistance = (float)((SPEED * SPEED) / acc2);
    // if (this->speed == 0)
    // {
    //     this->accelDistance = accDistance;
    //     // this->deccelDistance = accDistance;

    //     if (this->accelDistance >= halfDistance)
    //     {
    //         this->accelDistance = halfDistance;
    //         this->deccelDistance = halfDistance;
    //     }
    //     else
    //     {
    //         this->deccelDistance = this->accelDistance;
    //     }
    // }
    // else
    // {
    //     int speedPow = this->speed * this->speed;
    //     this->accelDistance = SPEED * SPEED - speedPow / ACCELERATION << 2;
    //     this->deccelDistance = accDistance;

    //     if (this->deccelDistance + this->accelDistance >= this->distance)
    //     {
    //         if (this->accelDistance < halfDistance)
    //         {
    //             this->accelDistance = this->accelDistance;
    //             this->deccelDistance = this->distance - this->accelDistance;
    //         }
    //         else
    //         {
    //             this->accelDistance = halfDistance;
    //             this->deccelDistance = halfDistance;
    //         }
    //     }
    // }

    // this->multiplier = (float)(ACCELERATION) / (float)((float)TICKS * (float)TICKS);
    // this->speed = sqrtf(acc2);
    // // original TICKS/sqrtf(acc2)
    // this->delayPeriod = TICKS / this->speed;
    // this->startTimer(this->delayPeriod);
};

bool AxisStepper::needsMoving()
{
    return this->distanceRun != this->distance;
};

void AxisStepper::startTimer(float frequency)
{
    #if defined(ARDUINO_GIGA)
    this->timer->attachInterruptInterval(this->delayPeriod, this->callback);
    #endif
    #if defined(ARDUINO_SAM_DUE)
    this->timer->attachInterrupt(this->callback).setPeriod(this->delayPeriod).start();
    #endif
    
};
