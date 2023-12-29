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

#define PADDLE_WIDTH 200 // 10 cm
#define PADDLE_HALF 100 // 5 cm
#define BALL_WIDTH 80
#define BALL_HALF 40