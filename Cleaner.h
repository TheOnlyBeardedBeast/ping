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

ClearTarget cleanTarget0, cleanTarget1, cleanTarget2, cleanTarget3;

ClearTarget clearTimes[4] = {cleanTarget0, cleanTarget1, cleanTarget2, cleanTarget3};

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
    }
    case 3:
    {
        p2Stepper.clearDirection();
    }
    // case 4:
    // {
    //     p1.clearSingleStep();
    // }
    // case 5:
    // {
    //     p2.clearSingleStep();
    // }
    default:
        return;
    }
}

void clearSteps()
{
    for (size_t i = 0; i < 4; i++)
    {
        if (clearTimes[i].enabled && (micros() - clearTimes[i].time) >= 10)
        {
            runClear(i);
            clearTimes[i].enabled = false;
        }
    }
}
