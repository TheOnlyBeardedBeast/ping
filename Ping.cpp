#include "Ping.h"
#include "utils.h"

#define PLAYER1_BUTTON 47
#define PLAYER2_BUTTON 46

void Ping::init(Ball *ball, Paddle *paddle1, Paddle *paddle2)
{
    this->ball = ball;
    this->paddles[0] = paddle1;
    this->paddles[1] = paddle2;
}

void Ping::calibrate()
{
    Paddle::calibrate();
    this->ball->calibrate();

    this->gameState = GameState::CENTER;
}

void Ping::center()
{
    Paddle::instances[0]->_stepper->setTarget(PADDLE_CENTER);
    Paddle::instances[1]->_stepper->setTarget(PADDLE_CENTER);

    this->ball->setposition(this->ball->limits.x >> 1, this->ball->limits.y >> 1, 500);

    this->gameState = GameState::CENTER_PROGRESS;
}

void Ping::centerProgress()
{
    if (Paddle::instances[0]->_stepper->position != PADDLE_CENTER || Paddle::instances[1]->_stepper->position != PADDLE_CENTER)
    {
        Paddle::instances[0]->_stepper->step();
        Paddle::instances[1]->_stepper->step();
        return;
    }

    if (this->ball->needsToMove())
    {
        return;
    }

    delay(500);
    // TODO: put the game into stand by mode so the game can be started by the ball shooting button
    // this->gameState = GameState::STAND_BY;
    this->gameState = GameState::MATCH_INIT;
}

void Ping::initMatch()
{
    if (this->lastWinner == Player::NOONE)
    {
        this->lastWinner = (Player)random(0, 2);
        this->shooter = this->lastWinner;
    }

    if (this->shooter == Player::Player1)
    {
        this->ball->setposition(this->ball->limits.x, (this->ball->limits.y >> 1) + paddles[this->shooter]->getCenterRelativePosition(), 500);
    }
    else
    {
        this->ball->setposition(0, (this->ball->limits.y >> 1) + paddles[this->shooter]->getCenterRelativePosition(), 500);
    }

    this->gameState = GameState::MATCH_INIT_PROGRESS;
}

void Ping::initMatchProgress()
{
    if (this->ball->needsToMove())
    {
        return;
    }

    this->gameState = GameState::MATCH_INIT_DONE;
}

void Ping::initMatchDone()
{
    ball->setDirection(StepDirection::NONE);

    paddles[this->shooter]->subscribe(Ball::setDirection, Ball::singleStep);

    Paddle::attachPaddles();

    this->gameState = GameState::MATCH_SERVE;
}

/// @brief Runs a serving step in the game, must be called in a loop
void Ping::serveMatch()
{
    size_t inputPin = this->shooter == 0 ? 47 : 46;

    if (!digitalRead(inputPin))
    {
        Paddle::instances[(int)this->shooter]->unsubScribe();
        delay(1);

        float modifier = this->shooter == Player::Player1 ? (float)map(Paddle::instances[(int)this->shooter]->getPosition(), 0, PADDLE_LIMIT, 6, 30) : (float)map(Paddle::instances[(int)this->shooter]->getPosition(), 0, PADDLE_LIMIT, 30, 6);
        float angle = ((modifier * 5.f) + (this->shooter == Player::Player1 ? 180.f : 0.0f));

        this->ball->shootDeg(angle);
        this->gameState = GameState::SERVE_PROGRESS;
        return;
    }

    return;
}

void Ping::serveProgress()
{
    Point ballPosition = this->ball->getPosition();
    Point ballLimits = this->ball->limits;

    if (ballLimits.x == ballPosition.x || 0 == ballPosition.x)
    {
        return;
    }

    this->gameState = GameState::MATCH_RUN;
    return;
}

void Ping::runMatch()
{
    if (this->ball->needsToMove())
    {
        return;
    }

    Point ballPosition = this->ball->getPosition();
    Point ballLimits = this->ball->limits;

    // POINT OR PADDLE HIT
    if (ballLimits.x == ballPosition.x || 0 == ballPosition.x)
    {

        Player nextShooter = this->shooter == Player::Player1 ? Player::Player2 : Player::Player1;

        byte shot = Paddle::instances[(int)nextShooter]->canShoot(ball->getCenterRelativePosition());

        if (shot != 0)
        {
            if (nextShooter == Player::Player2)
            {
                shot = map(shot, 30, 150, 150, 30);
            }

            float angle = (shot + (nextShooter == Player::Player1 ? 180.f : 0.0f));
            this->ball->shootDeg(angle);
            this->shooter = nextShooter;
            this->gameState = GameState::SERVE_PROGRESS;
            return;
        }

        Paddle::detachPaddles();

        score[this->shooter]++;

        if (score[0] == 5 || score[1] == 5)
        {
            delay(5000);

            score[0] = 0;
            score[1] = 0;

            this->shooter = Player::NOONE;
            this->gameState = GameState::CENTER;
        }

        this->shooter = nextShooter;
        this->gameState = GameState::MATCH_INIT;
        return;
    }

    // BOUNCE
    if ((ballLimits.y == ballPosition.y || 0 == ballPosition.y) && ballPosition.x != 0 && ballPosition.x != ballLimits.x)
    {
        ball->bounce();
        this->gameState = GameState::BOUNCE_PROGRESS;
        return;
    }

    return;
}

void Ping::endMatch()
{
    delay(1000);
    this->ball->center();

    this->gameState = GameState::CENTER_PROGRESS;
}

void Ping::bounceProgess()
{
    Point ballPosition = this->ball->getPosition();
    Point ballLimits = this->ball->limits;

    if (ballLimits.y == ballPosition.y || 0 == ballPosition.y)
    {
        return;
    }

    this->gameState = GameState::MATCH_RUN;
}
