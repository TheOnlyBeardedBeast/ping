#include "Paddle.h"
#include "Ball.h"

enum Player{
    NONE = -1,
    Player1 = 0,
    Player2 = 1
};

class Ping
{
public:
    void init();
    void run();
    void calibrate();
    void initMatch();

private:
    Paddle *paddleL = nullptr;
    Paddle *paddleR = nullptr;
    Ball *ball = nullptr;
    Player lastWinner = Player::NONE;
};