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
    // if(SERVE_PIN(this->lastWinner))
    // {
    if(this->shooter == Player::Player1)
    {
        auto angle = ((float)random(15, 166))/180.f*(float)PI;
        this->ball->shootAngle(angle);
    } else {
        auto angle = ((float)random(15, 166)+180.f)/180.f*(float)PI;
        this->ball->shootAngle(angle);
    }
    // 0.785398163
    // TODO: #improvement calculate a vector to serve a ball in an angle if the paddle is moving
    this->gameState = GameState::SERVE_PROGRESS;
    return;
    // }

    // Paddle* paddle = this->paddles[this->lastWinner];
    // int paddlePosition = this->paddles[this->lastWinner]->getPosition();

    // int mappedPosition = map(paddlePosition,0,paddle->limitMax,0,ball->limits.x);
    // this->ball->setposition(this->ball->getPosition().x,mappedPosition);
    // this->run();
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
        // Danger
        Serial.println("Danger");
        this->ball->stopNow();
        GameState::STAND_BY;
        return;
    }

    if(ballLimits.x - SAFEZONE_WIDTH <= ballPosition.x || SAFEZONE_WIDTH >= ballPosition.x)
    {
        delay(20);
        // Serial.println("Bounce wait");
        // Serial.println(this->ball->needsToMove());
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

    // TODO:
    // check if game point or wall hit
    // if vertical wall hit -> bounce
    // if horizontal wall hit -> game point
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

void Ping::centerProgress()
{
    while(this->ball->needsToMove())
    {
        delay(50);
        return;
    }

    delay(1000);
    this->gameState = GameState::MATCH_INIT;
}