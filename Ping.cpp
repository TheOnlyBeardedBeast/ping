#include "Ping.h"
#include "utils.h"

void Ping::init(Ball *ball, Paddle *paddle1)
{
    this->ball = ball;
    this->paddles[0] = paddle1;
}

void Ping::calibrate()
{
    // this->paddles[0]->runCalibration();
    // this->paddles[1]->runCalibration();
    this->ball->calibrate();

    // this->gameState = GameState::STAND_BY;
    this->gameState = GameState::MATCH_INIT;
}

void Ping::initMatch()
{
    if (this->lastWinner == Player::NONE)
    {
        this->lastWinner = (Player)random(0, 2);
        this->shooter = this->lastWinner;
    }

    Serial.print("Shooter:");
    Serial.println(this->shooter);

    if(this->shooter == Player::Player1) 
    {
        Serial.println("Center player 1");
        // shoots (0 pi)
        this->ball->setposition(this->ball->limits.x >> 1, SAFEZONE_WIDTH);
    } else {
        // shoots (pi,2pi)
        Serial.println("Center player 2");
        this->ball->setposition(this->ball->limits.x>>1,this->ball->limits.y);
    }

    Serial.println("Wait run start");
    ball->waitRun();
    Serial.println("Wait run end");

    // Paddle::attachPaddles();
    // this->gameState = GameState::STAND_BY;
    Paddle::attachPaddles();
    this->gameState = GameState::MATCH_SERVE;
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
    // if(this->shooter == Player::Player1)
    // {
    //     auto angle = ((float)random(15, 166))/180.f*(float)PI;
    //     this->ball->shootAngle(angle);
    //     this->gameState = GameState::SERVE_PROGRESS;
    // } else {
    //     auto angle = ((float)random(15, 166)+180.f)/180.f*(float)PI;
    //     this->ball->shootAngle(angle);
    //     this->gameState = GameState::SERVE_PROGRESS;
    // }
    
    return;
}

void Ping::serveProgress()
{
    Point ballPosition = this->ball->getPosition();
    Point ballLimits = this->ball->limits;

    if(ballLimits.y - SAFEZONE_WIDTH <= ballPosition.y || SAFEZONE_WIDTH >= ballPosition.y){
        delay(20);
        return;
    }

    this->gameState = GameState::MATCH_RUN;
}

void Ping::bounceProgess()
{
    Point ballPosition = this->ball->getPosition();
    Point ballLimits = this->ball->limits;

    if(ballLimits.x - 20 <= ballPosition.x || 20 >= ballPosition.x)
    {
        this->ball->stopNow();
        GameState::STAND_BY;
        return;
    }

    if(ballLimits.x - SAFEZONE_WIDTH <= ballPosition.x || SAFEZONE_WIDTH >= ballPosition.x)
    {
        delay(1);
        return;
    }

    this->gameState = GameState::MATCH_RUN;
}

void Ping::runMatch()
{
    Point ballPosition = this->ball->getPosition();
    Point ballLimits = this->ball->limits;

    if(ballLimits.x - 20 <= ballPosition.x || 20 >= ballPosition.x)
    {
        // Danger
        Serial.println("Danger");
        this->ball->stopNow();
        GameState::STAND_BY;
        return;
    }

    if (ballLimits.x - SAFEZONE_WIDTH <= ballPosition.x || SAFEZONE_WIDTH >= ballPosition.x)
    {
        // DEBUG
        Serial.println("Bounce hit");
        // END
        ball->stopNow();
        ball->bounce();
        this->gameState = GameState::BOUNCE_PROGRESS;
        return;
    }

    if (ballLimits.y - SAFEZONE_WIDTH <= ballPosition.y || SAFEZONE_WIDTH >= ballPosition.y)
    {
        Player nextShooter = this->shooter == Player::Player1 ? Player::Player2 : Player::Player1;

        // this->paddles[(int)nextShooter]->getPosition();
             


        this->shooter = this->shooter == Player::Player1 ? Player::Player2 : Player::Player1;
        this->ball->stopNow();
        this->gameState = GameState::MATCH_SERVE;
        Serial.println("Limit hit");
        return;
        // this->gameState = GameState::MATCH_END;
        // DEBUG
        // END
    }

    delay(10);
}

void Ping::centerProgress()
{
    while(this->ball->needsToMove())
    {
        delay(20);
        return;
    }

    delay(500);
    this->gameState = GameState::MATCH_INIT;
}