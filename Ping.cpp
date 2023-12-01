#include "Ping.h"
#include "utils.h"

void Ping::init(Ball *ball)
{
    this->ball = ball;
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
    if(this->lastWinner == Player::NONE) {
        this->lastWinner = (Player)random(0,2);
    }

    // if(this->lastWinner == Player::Player1) {
        this->ball->setposition(this->ball->limits.x>>1,400);
    // } else {
    //     this->ball->setposition(this->ball->limits.x>>1,this->ball->limits.y);
    // }

    ball->waitRun();

    Paddle::attachPaddles();
    this->gameState = GameState::MATCH_SERVE;
    // this->gameState = GameState::MATCH_SERVE;
}

void Ping::endMatch()
{
    this->ball->runCenter();

    // this->paddles[0]->center();
    // this->paddles[1]->center();

    // while (paddles[0]->needsToMove() || paddles[1]->needsToMove())
    // {
    //     paddles[0]->run();
    //     paddles[1]->run();
    // }
    

    this->gameState = GameState::MATCH_INIT;
}

/// @brief Runs a serving step in the game, must be called in a loop
void Ping::serveMatch()
{
    // if(SERVE_PIN(this->lastWinner))
    // {
        this->ball->shootAngle(0.523598776); //0.785398163
        // TODO: #improvement calculate a vector to serve a ball in an angle if the paddle is moving
        this->gameState = GameState::MATCH_RUN;
        return;
    // }

    // Paddle* paddle = this->paddles[this->lastWinner]; 
    // int paddlePosition = this->paddles[this->lastWinner]->getPosition();

    // int mappedPosition = map(paddlePosition,0,paddle->limitMax,0,ball->limits.x);
    // this->ball->setposition(this->ball->getPosition().x,mappedPosition);
    // this->run();
}

void Ping::runMatch()
{
    Point ballPosition = this->ball->getPosition();
    Point ballLimits = this->ball->limits;

    // if(ball->needsToMove()) {
    //     // TODO:
    //     // check paddle
    //     // if paddle hit then shoot
    //     return;
    // }
        // TODO:
        // check if game point or wall hit
        // if vertical wall hit -> bounce
        // if horizontal wall hit -> game point
     if(ballLimits.x <= ballPosition.x || 0 >= ballPosition.x)
    {
        ball->bounce();
    } 
     if(ballLimits.y <= ballPosition.y || 0 >= ballPosition.y)
    {
        this->gameState = GameState::MATCH_END;
        Paddle::detachPaddles();
    }
    // if(ballPosition.y <= 0+PADDLE_WIDTH || ballPosition.y > ballLimits.y -PADDLE_WIDTH)
    // {
    //     // TODO: separate sides
    //     // only calculate hit if the next position is going to the paddle
    //     // checking the vector also prevents multiple calls
    //     // check the x position as well
    // }
    // else if (true) 
    // {

    // }
    // else if((ballPosition.x >= ballLimits.x || ballPosition.x <= 0) && ballPosition.y > 0 && ballPosition.y < ballLimits.y)
    // {
    //     // TODO: hit the top or bottom wall
    // }
    // TODO: wall hit
    // TODO: match point {
    // TODO: set winner
        // this->gameState = GameState::MATCH_END;
    // }
    // TODO: paddle hit
}