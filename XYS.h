#include <Arduino.h>
#include "Portenta_H7_TimerInterrupt.h"
#include "helpers.h"


struct XYStepper {
    int step_pin;
    int dir_pin;
    uint16_t step_mask;
    uint16_t dir_mask;
    // TODO: store pin mask for step
    // TODO: store pin mask for dir
};

enum StepDirection {
    FORWARD = 1,
    BACKWARD = -1,
};

enum MainAxis {
    X,
    Y
};

class XYS {
    static XYS * instance;
    static void ballIsr();
    
    public:

    void moveWhile(PinStatus motor1,PinStatus motor2, unsigned short speed,BoolCallback condition);
    GPIO_TypeDef *port;
    void setTimer(Portenta_H7_Timer *timer);
    void init(int stepX,int dirX,int stepY,int dirY);
    void step();
    void setPosition(long x, long y);
    long getX() 
    {
        return this->x;
    };
    long getY() 
    {
        return this->y;
    };
    void setCurrentPosition(long x, long y)
    {
        this->x = x;
        this->y = y;
    };
    // void setPosition(long x, long y, int speed);
    bool needsMoving();
    void stop();
    void stopNow();

    void startTimer(float period);


    volatile bool moving = false;
    bool isMoving(){
        return this->moving;
    }
    StepDirection direction = StepDirection::FORWARD;
    // long targetX = 0;
    // long targetY = 0;
    long x = 0;
    long y = 0;
    long speed = 0;
    XYStepper stepperX;
    XYStepper stepperY;
    // long lastStepAt = 0;

    // Lieb ramp
    long acceleration = 0;
    long distance = 0;
    long distanceRun = 0;
    long accelDistance = 0;
    long deccelDistance = 0;
    float multiplier = 0;
    float delayPeriod = 0;

    void resetRamping() {
        this->acceleration = 0;
        this->distance = 0;
        this->distanceRun = 0;
        this->accelDistance = 0;
        this->deccelDistance = 0;
        this->multiplier = 0;
        this->delayPeriod = 0;
        this->speed = 0;
    }

    void printRamp()
    {
        Serial.print("Acceleration:");
        Serial.println(this->acceleration);
        Serial.print("Distance:");
        Serial.println(this->distance);
        Serial.print("DistanceRun:");
        Serial.println(this->distanceRun);
        Serial.print("Acceldistance:");
        Serial.println(this->accelDistance);
        Serial.print("Decceldistance:");
        Serial.println(this->deccelDistance);
        Serial.print("Multiplier:");
        Serial.println(this->multiplier);
        Serial.print("DelayPeriod:");
        Serial.println(this->delayPeriod);
        Serial.print("Speed:");
        Serial.println(this->speed);
    }


    // Bresenham
    int dx;
    int dy;
    int sx;
    int sy;
    int err;
    MainAxis axis;

    void resetBresenham()
    {
        this->dx = 0;
        this->dy = 0;
        this->sx = 0;
        this->sy = 0;
        this->err = 0;
    };

    void printBresenham()
    {
        Serial.print("Dx:");
        Serial.println(this->dx);
        Serial.print("dy:");
        Serial.println(this->dy);
        Serial.print("sx:");
        Serial.println(this->sx);
        Serial.print("sy:");
        Serial.println(this->sy);
        Serial.print("err:");
        Serial.println(this->err);
    }

    void stepLeft();
    void stepRight();

    // Timer
    Portenta_H7_Timer *timer;
};