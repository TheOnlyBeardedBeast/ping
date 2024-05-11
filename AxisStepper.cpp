#include "AxisStepper.h"
#if defined(ARDUINO_GIGA)
    #include "GigaDigitalWriteFast.h"
#elif defined(ARDUINO_SAM_DUE)
    #include "DueWriteFast.h"
#endif


#define SPEED 1600
#define ACCELERATION 10*5000
#define TICKS 1000000

#if defined(ARDUINO_GIGA)
    void AxisStepper::setTimer(Portenta_H7_Timer *timer)
    {
        this->timer = timer;
    };
#elif defined(ARDUINO_SAM_DUE)
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
};

void AxisStepper::singleStep()
{
    if(this->direction == AxisStepper::StepDirection::NONE)
    {
        return;
    }

    if(alternate){
        digitalWriteFast(this->stepper.step_pin, HIGH);
        alternate = false;
    } else {
        digitalWriteFast(this->stepper.step_pin, LOW);
        alternate = true;
    }
    // delayMicroseconds(3);
    // digitalWriteFast(this->stepper.step_pin, LOW);
}

void AxisStepper::step()
{
    if (!this->needsMoving())
    {
        #if defined(ARDUINO_GIGA)
            this->timer->stopTimer();
        #elif defined(ARDUINO_SAM_DUE)
            this->timer->stop();
        #endif
            this->timer->detachInterrupt();
       

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
    #elif defined(ARDUINO_SAM_DUE)
        this->timer->setPeriod(this->delayPeriod).start();
    #endif
    
    this->speed = TICKS / this->delayPeriod;
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
    #elif defined(ARDUINO_SAM_DUE)
        this->timer->stop();
    #endif
    this->timer->detachInterrupt();

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
    #elif defined(ARDUINO_SAM_DUE)
        this->timer->attachInterrupt(this->callback).setPeriod(this->delayPeriod).start();
    #endif  
};

void AxisStepper::setDirection(StepDirection dir)
{
    if(this->direction == dir) {
        return;
    }
    
    this->direction = dir;
    digitalWriteFast(this->stepper.dir_pin, dir > 0 ? HIGH : LOW);

    delayMicroseconds(5);
}
