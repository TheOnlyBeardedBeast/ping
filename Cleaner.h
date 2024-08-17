#pragma once

#include "AxisStepper.h"
#include "XYS.h"
#include "ClearTarget.h"
#include "Paddle.h"

extern AxisStepper p1Stepper;
extern AxisStepper p2Stepper;
extern XYS xyStepper;
extern Paddle p1;
extern Paddle p2;

ClearTarget cleanTarget0, cleanTarget1, cleanTarget2, cleanTarget3, cleanTarget4, cleanTarget5;

ClearTarget clearTimes[6] = {cleanTarget0, cleanTarget1, cleanTarget2, cleanTarget3, cleanTarget4, cleanTarget5};

void runClear(int target)
{
    switch (target)
    {
    case 0:
    {
        p1Stepper.clearStep();
        break;
    }
    case 1:
    {
        p2Stepper.clearStep();
        break;
    }
    case 2:
    {
        p1Stepper.clearDirection();
        break;
    }
    case 3:
    {
        p2Stepper.clearDirection();
        break;
    }
    case 4:
    {
        xyStepper.clearStep();
        break;
    }
    case 5:
    {
        xyStepper.clearDirection();
        break;
    }
    default:
        return;
    }
}

void clearSteps()
{
    for (size_t i = 0; i < 6; i++)
    {
        if (clearTimes[i].enabled && (micros() - clearTimes[i].time) >= 10)
        {
            runClear(i);
            clearTimes[i].enabled = false;
        }
    }
}
