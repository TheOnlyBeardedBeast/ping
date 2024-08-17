#include "Paddle.h"
#include "Ball.h"

enum Player
{
    NOONE = -1,
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
    MATCH_END,
    SERVE_PROGRESS,
    BOUNCE_PROGRESS,
    CENTER_PROGRESS
};

class Ping
{
public:
    GameState gameState = GameState::CALIBRATION;

    void init(Ball *ball, Paddle *paddle1, Paddle *paddle2);
    void calibrate();
    void initMatch();
    void serveMatch();
    void runMatch();
    void endMatch();
    void serveProgress();
    void bounceProgess();
    void centerProgress();

private:
    Paddle *paddles[2] = {nullptr, nullptr};
    Ball *ball = nullptr;
    Player lastWinner = Player::NOONE;
    Player shooter = Player::NOONE;
};