#include <AccelStepper.h>

struct Point
{
    int x;
    int y;
};

class Ball
{
public:
    void run();
    void setposition(int x, int y);
    void stop();
    Point getPosition();

private:
    AccelStepper *_stepperA = NULL;
    AccelStepper *_stepperB = NULL;
};