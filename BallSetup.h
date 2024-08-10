#include "Ball.h"

XYS xyStepper;

#if defined(ARDUINO_GIGA)
Portenta_H7_Timer xyTimer(TIM15);
#elif defined(ARDUINO_SAM_DUE)
DueTimer xyTimer = Timer.getAvailable();
#endif

extern Ball ball;

void setupBall()
{
    // Motor A and B STEP and DIRECTION pins
    pinMode(BALLSTEPPER_A_STP, OUTPUT);
    pinMode(BALLSTEPPER_A_DIR, OUTPUT);
    pinMode(BALLSTEPPER_B_STP, OUTPUT);
    pinMode(BALLSTEPPER_B_DIR, OUTPUT);

    // Limit switches for X and Y axes
    pinMode(LS1, INPUT_PULLUP);
    pinMode(LS2, INPUT_PULLUP);
    pinMode(LS3, INPUT_PULLUP);
    pinMode(LS4, INPUT_PULLUP);

    xyStepper.init(BALLSTEPPER_A_STP, BALLSTEPPER_A_DIR, BALLSTEPPER_B_STP, BALLSTEPPER_B_DIR);
    xyStepper.setTimer(&xyTimer);

    ball.setMotors(&xyStepper);
}