#include "Paddle.h"
#include "Ball.h"

enum Player{
    NONE = -1,
    Player1 = 0,
    Player2 = 1
};

enum GameState 
{
    CALIBRATION,
    STAND_BY,
    MATCH_INIT,
    MATCH_SERVE,
    MATCH_RUN,
    MATCH_END
};

class Ping
{
public:
    GameState gameState = GameState::CALIBRATION;
    
    void init(Ball *ball);
    void calibrate();
    void initMatch();
    void serveMatch();
    void runMatch();
    void endMatch();

private:
    Paddle *paddles[2] = {nullptr,nullptr};
    Ball *ball = nullptr;
    Player lastWinner = Player::NONE;
};