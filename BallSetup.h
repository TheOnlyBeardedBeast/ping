#include "Ball.h"

XYS xyStepper;

#if defined(ARDUINO_GIGA)
    Portenta_H7_Timer xyTimer(TIM15);
#elif defined(ARDUINO_SAM_DUE)
    DueTimer xyTimer = Timer.getAvailable();
#endif

extern Ball ball;

void ballIsr()
{
    xyStepper.step();
}

void setupBall()
{
    // Motor A and B STEP and DIRECTION pins
    pinMode(26, OUTPUT);
    pinMode(27, OUTPUT);
    pinMode(28, OUTPUT);
    pinMode(29, OUTPUT);

    // Limit switches for X and Y axes
    pinMode(LS1, INPUT_PULLUP);
    pinMode(LS1, INPUT_PULLUP);
    pinMode(LS3, INPUT_PULLUP);
    pinMode(LS4, INPUT_PULLUP);

    xyStepper.init(26, 27, 28, 29);
    xyStepper.setTimer(&xyTimer);
    xyStepper.setIsr(ballIsr);

    ball.setMotors(&xyStepper);
    
}