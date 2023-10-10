#include <AccelStepper.h>

struct Point
{
    int x;
    int y;
};

class Ball
{
public:
    void setMotors(AccelStepper *_stepperA, AccelStepper *_stepperB);
    void run();
    void setposition(int x, int y);
    void runSpeed();

    void stop();
    void calibrate();
    Point getPosition();
    bool calibrationState[4] = {false,false,false,false};
    
private:
    AccelStepper *_stepperA = nullptr;
    AccelStepper *_stepperB = nullptr;

    int maxX = 0;
    int maxY = 0;
    
};