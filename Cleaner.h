#pragma once

#include "AxisStepper.h"
#include "XYS.h"
#include "ClearTarget.h"

extern AxisStepper p1Stepper;
extern AxisStepper p2Stepper;
extern XYS xyStepper;

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
    default:
        return;
    }
}

void clearSteps()
{
    for (size_t i = 0; i < 4; i++)
    {
        if (clearTimes[i].enabled && (micros() - clearTimes[i].time) >= 5)
        {
            runClear(i);
            clearTimes[i].enabled = false;
        }
    }
}
