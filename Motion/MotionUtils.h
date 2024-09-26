#pragma once

#include "../Models/Directable.h"

StepDirection diffToDirection(int16_t diff)
{
    if (diff < 0)
    {
        return StepDirection::BACKWARD;
    }

    if (diff > 0)
    {
        return StepDirection::FORWARD;
    }

    return StepDirection::NONE;
};

uint16_t calculateRampSteps(uint32_t speedPower, uint32_t edgeSpeedPower, uint32_t acceleration2)
{
    return static_cast<uint16_t>((speedPow - edgeSpeedPower) / acceleration2);
}

byte dirToPin(StepDirection dir)
{
    if (dir == StepDirection::FORWARD)
    {
        return HIGH;
    }

    return LOW;
}

uint32_t calculateAccelerationSpeed(uint32_t speedPower, uint32_t edgeSpeed, uint32_t acceleration2)
{
    return max(sqrt(acceleration2 + speedPower), edgeSpeedPower);
}

uint32_t calculateDeccelerationSpeed(uint32_t speedPower, uint32_t edgeSpeed, uint32_t acceleration2)
{
    max(sqrt(speedPower - acceleration2), edgeSpeedPower)
}