#pragma once
#include "AxisStepper.h"

#define Q_FORWARD 0b11111111111111111111111111111111
#define Q_BACKWARD 0
#define Q_INITIAL 0b10101010101010101010101010101010

/// @brief A class which smooths out accidental bounces
class DirectionSmoother
{
public:
    explicit DirectionSmoother();

    AxisStepper::StepDirection smoothDirection(AxisStepper::StepDirection input);

    AxisStepper::StepDirection getCurrentDirection();

    void clear();

private:
    uint32_t q = Q_INITIAL;
};