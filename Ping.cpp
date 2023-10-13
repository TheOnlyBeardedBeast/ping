#include "Ping.h"

void Ping::calibrate()
{
    this->ball->calibrate();
}

void Ping::initMatch()
{
    byte player;

    if(this->lastWinner == Player::NONE) {
        player = (byte)random(0,2);
    } else {
        player = this->lastWinner;
    }

    if(player == Player::Player1) {
        this->ball->setposition(0,this->ball->limits.y>>1);
    } else {
        this->ball->setposition(this->ball->limits.x,this->ball->limits.y>>1);
    }

    while (this->ball->needsToMove())
    {
        ball->run();
    }
    
}