#include <AccelStepper.h>

struct Point
{
    int x;
    int y;
};

class Ball
{
public:
    /// @brief An array which represents the limit switches visiting state
    bool calibrationState[4] = {false,false,false,false};
    Point limits;

    /// @brief Sets the motors which moves the balls
    /// @param _stepperA 
    /// @param _stepperB 
    void setMotors(AccelStepper *_stepperA, AccelStepper *_stepperB);

    /// @brief Call the run function for each stepper which moves the ball
    void run();

    /// @brief Sets an absolute position to go to with the default speed
    /// @param x 
    /// @param y 
    void setposition(int x, int y);

    /// @brief Sets an absolute position to go to with a custom speed
    /// @param x 
    /// @param y 
    /// @param speed 
    void setposition(int x, int y, int speed);

    /// @brief Stops the motors which moves the ball
    void stop();

    /// @brief Calibrates the game setting the min and max limits
    void calibrate();

    /// @brief Gets the current cartesian position of the ball from corexy/hbot positioning
    /// @return Poit which contains an X and an Y integer
    Point getPosition();

    void center();

    bool needsToMove();
private:
    AccelStepper *_stepperA = nullptr;
    AccelStepper *_stepperB = nullptr;

    void initCalibration();
};