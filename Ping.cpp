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
    Paddle::instances[0]->_stepper->setTarget(980);
    Paddle::instances[1]->_stepper->setTarget(980);

    this->ball->setposition(this->ball->limits.x >> 1, this->ball->limits.y >> 1);

    this->gameState = GameState::CENTER_PROGRESS;
}

void Ping::centerProgress()
{
    if (Paddle::instances[0]->_stepper->position != 980 || Paddle::instances[1]->_stepper->position != 980)
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
        this->ball->setposition(this->ball->limits.x, this->ball->limits.y >> 1);
    }
    else
    {
        this->ball->setposition(0, this->ball->limits.y >> 1);
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

    paddles[this->shooter]->setDirectionListener(Ball::setDirection);
    paddles[this->shooter]->setStepListener(Ball::singleStep);

    Paddle::attachPaddles();

    this->gameState = GameState::MATCH_SERVE;
}

/// @brief Runs a serving step in the game, must be called in a loop
void Ping::serveMatch()
{

    // shooter index is either 0 or 1
    size_t inputPin = this->shooter == 0 ? 47 : 46;

    if (!digitalRead(inputPin))
    {
        Paddle::instances[(int)this->shooter]->setDirectionListener(NULL);
        Paddle::instances[(int)this->shooter]->setStepListener(NULL);
        delay(1);

        float modifier = (float)map(paddles[(int)this->shooter]->getPosition(), 0, 1960, 165, 15);
        float angle = (modifier + (this->shooter == Player::Player1 ? 180.f : 0.0f)) * (float)PI / 180.f;
        // float angle = 270 * (float)PI / 180.f;
        this->ball->shootAngle(angle);
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
    Point ballPosition = this->ball->getPosition();
    Point ballLimits = this->ball->limits;

    // BOUNCE
    if (ballLimits.y == ballPosition.y || 0 == ballPosition.y)
    {
        ball->bounce();
        this->gameState = GameState::BOUNCE_PROGRESS;
        return;
    }

    // POINT OR PADDLE HIT
    if (ballLimits.x == ballPosition.x || 0 == ballPosition.x)
    {
        Player nextShooter = this->shooter == Player::Player1 ? Player::Player2 : Player::Player1;

        byte shot = this->paddles[(int)nextShooter]->canShoot(ball->getCenterRelativePosition());
        if (shot != 0)
        {
            float angle = (shot + (nextShooter == Player::Player1 ? 180.f : 0.0f)) * (float)PI / 180.f;
            this->ball->shootAngle(angle);
            this->shooter = nextShooter;
            this->gameState = GameState::SERVE_PROGRESS;
            return;
        }

        Paddle::detachPaddles();
        this->shooter = nextShooter;
        this->gameState = GameState::MATCH_INIT;
        return;
    }

    return;
}

void Ping::endMatch()
{
    delay(1000);
    this->ball->center();

    // this->paddles[0]->center();
    // this->paddles[1]->center();

    // while (paddles[0]->needsToMove() || paddles[1]->needsToMove())
    // {
    //     paddles[0]->run();
    //     paddles[1]->run();
    // }

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
