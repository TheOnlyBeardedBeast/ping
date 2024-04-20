#include "DirectionSmoother.h"

DirectionSmoother::DirectionSmoother()
{
    this->q = Q_INITIAL;
}

AxisStepper::StepDirection DirectionSmoother::smoothDirection(AxisStepper::StepDirection input)
{
    if(input == AxisStepper::StepDirection::NONE)
    {
        return AxisStepper::StepDirection::NONE;
    }

    if(input == AxisStepper::StepDirection::BACKWARD) {
        this->q = this->q >> 1;
    } else {
        this->q = (this->q << 1) & 1;
    }

    return this->getCurrentDirection();   
}

AxisStepper::StepDirection DirectionSmoother::getCurrentDirection()
{
    if (this->q == Q_BACKWARD)
    {
        return AxisStepper::StepDirection::BACKWARD;
    } 
    
    if(this->q == Q_FORWARD)
    {
        return AxisStepper::StepDirection::FORWARD;
    }

    return AxisStepper::StepDirection::NONE;
}

void DirectionSmoother::clear()
{
    this->q = Q_INITIAL;
}
