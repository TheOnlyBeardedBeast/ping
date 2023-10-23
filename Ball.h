#include <AccelStepper.h>

struct Point
{
    int x;
    int y;
};

enum BouncPosition
{
    TOP,
    BOTTOm,
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

    void init();

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

    /// @brief Sets the ball ball position to the playfield center
    void center();

    /// @brief Center the ball on the playfield and waits until the ball is positioned
    void runCenter();

    /// @brief Checks if the ball has any distance to make
    /// @return return true or false
    bool needsToMove();

    /// @brief angle the ball used last time
    float lastAngle;
    float lastVectorX;
    float lastVectorY;
    float lastSpeed;

    /// @brief Bounces onn the limits 
    void bounce();

    /// @brief Shoots a ball in an angle and sets its position to the limits
    /// @param radians defines the angle of shooting
    void shootAngle(double radians);

    void waitRun();
private:
    AccelStepper *_stepperA = nullptr;
    AccelStepper *_stepperB = nullptr;

    void initCalibration();
    void postCalibrationStop();

    int dx;
    int dy;
    int sx;
    int sy;
    int err;
    
};