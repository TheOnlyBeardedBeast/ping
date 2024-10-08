#include "AxisStepper.h"
#if defined(ARDUINO_GIGA)
#include "GigaDigitalWriteFast.h"
#elif defined(ARDUINO_SAM_DUE)
#include "DueWriteFast.h"
#endif

#include "ClearTarget.h"

extern ClearTarget clearTimes[4];

#define SIDE_LIMIT 980
#define SPEED 1600
#define ACCELERATION 10 * 5000
#define TICKS 1000000

// #if defined(ARDUINO_GIGA)
// void AxisStepper::setTimer(Portenta_H7_Timer *timer)
// {
//     this->timer = timer;
// };
// #elif defined(ARDUINO_SAM_DUE)
// void AxisStepper::setTimer(DueTimer *timer)
// {
//     this->timer = timer;
// };
// #endif

void AxisStepper::init(int step, int dir)
{
    pinMode(step, OUTPUT);
    this->stepper.step_pin = step;
    pinMode(dir, OUTPUT);
    this->stepper.dir_pin = dir;
};

bool AxisStepper::singleStep()
{
    if (clearTimes[this->id].enabled || clearTimes[this->id + 2].enabled || this->direction == StepDirection::NONE)
    {
        return false;
    }

    if (this->calibrated && ((this->direction == StepDirection::BACKWARD && this->position == 0) || (this->direction == StepDirection::FORWARD && this->position == SIDE_LIMIT)))
    {
        return false;
    }

    this->position += this->direction;
    digitalWriteFast(this->stepper.step_pin, HIGH);
    clearTimes[this->id].enabled = true;
    clearTimes[this->id].time = micros();

    return true;
}

void AxisStepper::step()
{
    if (!this->needsMoving())
    {
        this->resetRamping();
        this->_isRunning = false;

        return;
    }

    if (micros() < this->nextStepAt || clearTimes[this->id + 2].enabled)
    {
        return;
    }

    this->singleStep();
    this->distanceRun++;

    this->nextStepAt = micros() + 800;
};

void AxisStepper::setTarget(long newPosition)
{
    this->_isRunning = true;

    long diff = this->position - newPosition;
    this->distanceRun = 0;

    if (diff < 0)
    {
        this->setDirection(StepDirection::FORWARD);
    }
    else
    {
        this->setDirection(StepDirection::BACKWARD);
    }

    this->distance = abs(diff);
    this->nextStepAt = micros();
}

void AxisStepper::stop()
{
    this->distance = this->distanceRun;
}
void AxisStepper::forceStop()
{
    this->distanceRun = 0;
}

void AxisStepper::setSpeed(long speed)
{
    if (!this->isRunning())
    {
        return;
    }
};

bool AxisStepper::needsMoving()
{
    return this->distanceRun != this->distance;
};

void AxisStepper::setDirection(StepDirection dir)
{
    if (this->direction == dir)
    {
        return;
    }

    this->direction = dir;
    if (this->id == 0)
    {
        digitalWrite(this->stepper.dir_pin, dir > 0 ? HIGH : LOW);
    }
    else
    {
        digitalWrite(this->stepper.dir_pin, dir > 0 ? LOW : HIGH);
    }

    clearTimes[this->id + 2].enabled = true;
    clearTimes[this->id + 2].time = micros();
}

void AxisStepper::clearStep()
{
    digitalWriteFast(this->stepper.step_pin, LOW);
}

void AxisStepper::clearDirection()
{
}
