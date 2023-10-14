#include "Ping.h"

void Ping::calibrate()
{
    this->paddleL->runCalibration();
    this->paddleR->runCalibration();
    this->ball->calibrate();
}

void Ping::initMatch()
{
    if(this->lastWinner == Player::NONE) {
        this->lastWinner = (Player)random(0,2);
    }

    if(this->lastWinner == Player::Player1) {
        this->ball->setposition(0,this->ball->limits.y>>1);
    } else {
        this->ball->setposition(this->ball->limits.x,this->ball->limits.y>>1);
    }

    while (this->ball->needsToMove())
    {
        ball->run();
    }   

    this->gameState = GameState::MATCH_SERVE;
}

void Ping::endMatch()
{
    this->ball->center();
    // TODO: create an async centering so the padles can center in the same time;
    this->paddleL->center();
    this->paddleR->center();

    this->gameState = GameState::MATCH_INIT;
}

void Ping::serveMatch()
{
    if(digitalRead(42))
    {
        // TODO: set ball position
        this->gameState = GameState::MATCH_RUN;
        return;
    }

    // Get active player
    this->ball->setposition(this->ball->getPosition().x,this->paddleL->getPosition());
    this->run();
}

void Ping::run()
{
    this->ball->run();
    this->paddleL->run();
    this->paddleR->run();
}

void Ping::runMatch()
{
    // TODO: wall hit
    // TODO: match point {
    // TODO: set winner
        // this->gameState = GameState::MATCH_END;
    // }
    // TODO: paddle hit

    this->run();
}