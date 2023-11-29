#include "utils.h"
#include "Ping.h"
#include <Arduino.h>

Ball ball;
Ping ping;

XYS xyStepper;
#if defined(ARDUINO_GIGA)
Portenta_H7_Timer xyTimer(TIM15);
#endif
#if defined(ARDUINO_SAM_DUE)
DueTimer xyTimer = DueTimer(0);
#endif


void ballTimerIsr()
{
  xyStepper.step();
}

void setup()
{ 
  pinMode(26,OUTPUT);
  pinMode(27,OUTPUT);
  pinMode(28,OUTPUT);
  pinMode(29,OUTPUT);

  pinMode(32,INPUT_PULLUP);
  pinMode(33,INPUT_PULLUP);
  pinMode(34,INPUT_PULLUP);
  pinMode(35,INPUT_PULLUP);

  
  xyStepper.init(26,27,28,29);
  xyStepper.setTimer(&xyTimer);
  xyStepper.setCallback(ballTimerIsr);
  delay(2000);

  ball.setMotors(&xyStepper);
  ping.init(&ball);
}

void loop()
{
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
  case GameState::MATCH_RUN:
    ping.runMatch();
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

