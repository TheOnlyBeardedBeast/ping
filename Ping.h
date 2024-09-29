#include "Paddle.h"
#include "Ball.h"
#include "ScoreBoard/ScoreBoard.h"

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
    MATCH_INIT_PROGRESS,
    MATCH_INIT_DONE,
    MATCH_SERVE,
    MATCH_SERVE_PROGRESS,
    MATCH_RUN,
    MATCH_END,
    SERVE_PROGRESS,
    BOUNCE_PROGRESS,
    HIT_PROGRESS,
    CENTER,
    CENTER_PROGRESS
};

class Ping
{
public:
    GameState gameState = GameState::CALIBRATION;
    void init(Ball *ball, Paddle *paddle1, Paddle *paddle2);
    void calibrate();
    void initMatch();
    void initMatchProgress();
    void initMatchDone();
    void serveMatch();
    void runMatch();
    void endMatch();
    void serveProgress();
    void bounceProgess();
    void center();
    void centerProgress();

private:
    Paddle *paddles[2] = {nullptr, nullptr};
    Ball *ball = nullptr;
    Player lastWinner = Player::NOONE;
    Player shooter = Player::NOONE;
    ScoreBoard score;
};