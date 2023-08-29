#include "DirectionSmoother.h"

DirectionSmoother::DirectionSmoother()
{
    threshold = 5;
    this->inputHistory = ADeque<int>(20);
}

DirectionSmoother::DirectionSmoother(int threshold)
{
    this->threshold = threshold;
}

bool DirectionSmoother::smoothDirection(int input)
{
    this->inputHistory.push_back(input);

    if (this->inputHistory.count() > threshold)
    {
        this->inputHistory.pop_front();
    }

    int countZero = 0, countOne = 0;
    for (int i = 0; i < this->inputHistory.count(); i++)
    {
        int value = this->inputHistory[i];

        if (value == 0)
        {
            countZero++;
        }
        else
        {
            countOne++;
        }
    }

    if (countZero == threshold)
    {
        this->currentDirection = 0;
        return true;
    }
    else if (countOne == threshold)
    {
        this->currentDirection = 1;
        return true;
    }

    return false;
}

int DirectionSmoother::getCurrentDirection()
{
    this->currentDirection = -1;
    return this->currentDirection;
}

void DirectionSmoother::clear()
{
    this->inputHistory.clear();
}
