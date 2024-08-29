#pragma once

#if defined(ARDUINO_GIGA)
#define LS1 30
#define LS2 32
#define LS3 33
#define LS4 34
#elif defined(ARDUINO_SAM_DUE)
#define LS1 33
#define LS2 34
#define LS3 35
#define LS4 36
#endif

#define BALLSTEPPER_A_STP 26
#define BALLSTEPPER_A_DIR 27
#define BALLSTEPPER_B_STP 28
#define BALLSTEPPER_B_DIR 29

// 1step = 0.025cm
#define PADDLE_WIDTH 150 // 7.5 cm
#define PADDLE_WIDTH_HALF 75
#define BALL_WIDTH 70 // 3.5 cm, will change to 2.5cm
#define BALL_WIDTH_HALF 35