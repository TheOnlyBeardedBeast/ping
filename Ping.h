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
    MATCH_INIT,
    MATCH_SERVE,
    MATCH_RUN,
    MATCH_END
};

class Ping
{
public:
    GameState gameState = GameState::CALIBRATION;
    
    void init();
    void run();
    void calibrate();
    void initMatch();
    void serveMatch();
    void runMatch();
    void endMatch();

private:
    Paddle *paddleL = nullptr;
    Paddle *paddleR = nullptr;
    Ball *ball = nullptr;
    Player lastWinner = Player::NONE;
};