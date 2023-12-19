#include "Ball.h"

XYS xyStepper;
Portenta_H7_Timer xyTimer(TIM15);
Ball ball;

void setupBall()
{
    // Motor A and B STEP and DIRECTION pins
    pinMode(26, OUTPUT);
    pinMode(27, OUTPUT);
    pinMode(28, OUTPUT);
    pinMode(29, OUTPUT);

    // Limit switches for X and Y axes
    pinMode(30, INPUT_PULLUP);
    pinMode(32, INPUT_PULLUP);
    pinMode(34, INPUT_PULLUP);
    pinMode(36, INPUT_PULLUP);

    xyStepper.init(26, 27, 28, 29);
    xyStepper.setTimer(&xyTimer);

    ball.setMotors(&xyStepper);
}