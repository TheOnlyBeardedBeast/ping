#include "Paddle.h"
#include "Ball.h"

class Ping
{
public:
    void init();
    void run();

private:
    Paddle *paddleL = NULL;
    Paddle *paddleR = NULL;
    Ball *ball = NULL;
};