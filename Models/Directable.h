#pragma once

enum StepDirection
{
    FORWARD = 1,
    BACKWARD = -1,
    NONE = 0
};

class Directable
{
public:
    StepDirection direction = StepDirection::NONE;
    virtual void setDirection(StepDirection dir) = 0;
    virtual void clearDirection() = 0;
};