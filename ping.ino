#include "utils.h"
#include "Ping.h"
#include <Arduino.h>
#include "BallSetup.h"

Ping ping;

GameState prevState = GameState::STAND_BY;

void setup()
{ 

  // Serial.begin(115200);

  // while (!Serial)
  // {
  //   delay(10);
  // }
  

  pinMode(LEDB,OUTPUT);
  pinMode(LEDR,OUTPUT);
  setupBall();

  randomSeed(analogRead(0));
  delay(10000);

  ping.init(&ball);
}

void loop()
{
  

  // if(prevState!=ping.gameState){
  //   // DEBUG
  //   Serial.println("State change");
  //   Serial.print("x:");
  //   Serial.println(ball.getPosition().x);
  //   Serial.print("y:");
  //   Serial.println(ball.getPosition().y);
  //   // END
  //   prevState = ping.gameState;
  // }

  switch (ping.gameState)
  {
  case GameState::CALIBRATION:
    ping.calibrate();
    return;
  case GameState::MATCH_INIT:
    ping.initMatch();
    return;
  case GameState::MATCH_SERVE:
    ping.serveMatch();
    return;
  case GameState::SERVE_PROGRESS:
    ping.serveProgress();
    return;
  case GameState::MATCH_RUN:
    ping.runMatch();
    return;
  case GameState::BOUNCE_PROGRESS:
    ping.bounceProgess();
    return;
  default:
    return;
  }
}


// void loop()
// {
//   if(stepperX.distanceToGo() == 0 && stepperY.distanceToGo() == 0){
//     switch (last)
//     {
//     case 0:
//       ball.setposition(0,1200);
//       last = 1;
//       break;
//     case 1:
//       ball.setposition(800,1200);
//       last = 2;
//       break;
//     case 2:
//       ball.setposition(800,0);
//       last = 3;
//       break;
//     case 3:
//       ball.setposition(0,0);
//       last = 0;
//       break;

//     default:
//       break;
//     }
//   }

//   ball.run();
// }

