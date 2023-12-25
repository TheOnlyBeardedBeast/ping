#include "utils.h"
#include "Ping.h"
#include <Arduino.h>
#include "BallSetup.h"

Ping ping;
Ball ball;

GameState prevState = GameState::STAND_BY;

void setup()
{ 
  Serial.begin(115200);

  while (!Serial)
  {
    delay(10);
  }

  #if defined(ARDUINO_GIGA)
    pinMode(LEDB,OUTPUT);
  #elif defined(ARDUINO_SAM_DUE)
      pinMode(LED_BUILTIN,OUTPUT);
  #endif
  // pinMode(LEDR,OUTPUT);
  setupBall();

  randomSeed(analogRead(0));
  delay(10000);

  
  #if defined(ARDUINO_GIGA)
    digitalWrite(LEDB,LOW);
  #elif defined(ARDUINO_SAM_DUE)
    digitalWrite(LED_BUILTIN,LOW);
  #endif

  ping.init(&ball);
}

void loop()
{
  

  if(prevState!=ping.gameState){
    // DEBUG
    Serial.print("State change:");
    Serial.println(ping.gameState);
    Serial.print("x:");
    Serial.println(ball.getPosition().x);
    Serial.print("y:");
    Serial.println(ball.getPosition().y);
    // END
    prevState = ping.gameState;
  }

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
  case GameState::CENTER_PROGRESS:
    ping.centerProgress();
    return;
  case GameState::MATCH_END:
    ping.endMatch();
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

