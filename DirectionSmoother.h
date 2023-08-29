#include "ADeque.h"

/// @brief A class which smooths out accidental bounces
class DirectionSmoother
{
public:
    DirectionSmoother();

    explicit DirectionSmoother(int _threshold);

    bool smoothDirection(int input);

    int getCurrentDirection();

    void clear();

private:
    int threshold = 5;
    ADeque<int> inputHistory;
    int currentDirection = -1;
};