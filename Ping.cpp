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

    // this->gameState = GameState::STAND_BY;
    this->ball->calibrate();

    Paddle::instances[0]->_stepper->setTarget(980);
    Paddle::instances[1]->_stepper->setTarget(980);
    // Paddle::attachPaddles();
    // this->ball->setposition(this->ball->limits.x, this->ball->limits.y >> 1);
    this->ball->setposition(this->ball->limits.x >> 1, this->ball->limits.y >> 1);

    // this->gameState = GameState::STAND_BY;
    this->gameState = GameState::CENTER_PROGRESS;
    return;
}

void Ping::initMatch()
{
    // this->lastWinner = Player::Player1;
    // this->shooter = Player::Player1;
    if (this->lastWinner == Player::NOONE)
    {
        this->lastWinner = (Player)random(0, 2);
        this->shooter = this->lastWinner;
    }

    // Serial.print("Shooter:");
    // Serial.println(this->shooter);

    if (this->shooter == Player::Player1)
    {
        // Serial.println("Center player 1");
        // shoots (0 pi)
        this->ball->setposition(this->ball->limits.x, (this->ball->limits.y >> 1) + paddles[0]->getCenterRelativePosition());
    }
    else
    {
        // shoots (pi,2pi)
        // Serial.println("Center player 2");
        this->ball->setposition(0, (this->ball->limits.y >> 1) + paddles[1]->getCenterRelativePosition());
    }

    // Serial.println("Wait run start");
    ball->waitRun();
    // Serial.println("Wait run end");
    ball->setDirection(StepDirection::NONE);
    paddles[this->shooter]->setDirectionListener(Ball::setDirection);
    paddles[this->shooter]->setStepListener(Ball::singleStep);
    Paddle::attachPaddles();
    // this->gameState = GameState::STAND_BY;
    this->gameState = GameState::MATCH_SERVE;
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

/// @brief Runs a serving step in the game, must be called in a loop
void Ping::serveMatch()
{

    // shooter index is either 0 or 1
    float shooterIndex = (float)this->shooter;
    size_t inputPin = this->shooter == 0 ? 47 : 46;

    if (!digitalRead(inputPin))
    {
        Paddle::instances[(int)this->shooter]->setDirectionListener(NULL);
        Paddle::instances[(int)this->shooter]->setStepListener(NULL);
        delay(1);

        float modifier = (float)map(paddles[this->shooter]->getPosition(), 0, 1960, 165, 15);
        float angle = (modifier + (shooterIndex == 0 ? 180.f : 0.0f)) * (float)PI / 180.f;
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

        // this->paddles[(int)nextShooter]->getPosition();

        this->shooter = this->shooter == Player::Player1 ? Player::Player2 : Player::Player1;
        // this->ball->stopNow();
        this->gameState = GameState::MATCH_SERVE;
        return;
        // this->gameState = GameState::MATCH_END;
        // DEBUG
        // END
    }

    return;
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