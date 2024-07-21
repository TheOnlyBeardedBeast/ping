#include "utils.h"
#include "Ping.h"
#include <Arduino.h>
#include "BallSetup.h"
#include "Cleaner.h"

Ping ping;
Ball ball;
Paddle p1;
Paddle p2;

AxisStepper p1Stepper;
AxisStepper p2Stepper;

GameState prevState = GameState::STAND_BY;

DueTimer clearTimer = Timer.getAvailable();

bool TESTED = false;

void clearStep()
{
  micros();
}

void setup()
{
  // Serial.begin(115200);
  // pinMode(LED_BUILTIN,OUTPUT);

  // while (!Serial);

  // Serial.println("available");

  // #if defined(ARDUINO_GIGA)
  //   pinMode(LEDB,OUTPUT);
  // #elif defined(ARDUINO_SAM_DUE)
  //     pinMode(LED_BUILTIN,OUTPUT);
  // #endif
  // // pinMode(LEDR,OUTPUT);
  // setupBall();

  randomSeed(analogRead(0));

  p1.initializeEncoder(48, 49);
  p1Stepper.init(42, 43);
  p1.initializeStepper(&p1Stepper);
  p1Stepper.id = 0;

  p2.initializeEncoder(18, 19);
  p2Stepper.init(20, 21);
  p2.initializeStepper(&p2Stepper);
  p2Stepper.id = 1;

  Paddle::instances[0] = &p1;
  Paddle::instances[1] = &p2;
  Paddle::attachPaddles();

  clearTimer.attachInterrupt(clearSteps);
  clearTimer.setPeriod(5);
  clearTimer.start();

  delay(1000);

  // delay(5000);

  // #if defined(ARDUINO_GIGA)
  //   digitalWrite(LEDB,LOW);
  // #elif defined(ARDUINO_SAM_DUE)
  //   digitalWrite(LED_BUILTIN,LOW);
  // #endif

  // ping.init(&ball,&p1);

  // Serial.println("Run");
  // for(int i = 0; i<50;i++)
  // {
  //   digitalWriteFast(42,HIGH);
  //   delayMicroseconds(3);
  //   digitalWriteFast(42,LOW);
  //   delay(20);
  // }
  pinMode(LS3, INPUT_PULLUP);
  pinMode(LS4, INPUT_PULLUP);
}

void loop()
{
  if (!p1._stepper->calibrated)
  {
    p1._stepper->setDirection(AxisStepper::StepDirection::BACKWARD);
    while (digitalRead(LS3))
    {
      p1._stepper->singleStep();
      delayMicroseconds(750);
    }
    p1._stepper->calibrated = true;
    p1._stepper->setCurrentPosition(0);
  }

  if (!p2._stepper->calibrated)
  {
    p2._stepper->setDirection(AxisStepper::StepDirection::BACKWARD);
    while (digitalRead(LS4))
    {
      p2._stepper->singleStep();
      delayMicroseconds(750);
    }
    p2._stepper->calibrated = true;
    p2._stepper->setCurrentPosition(0);
  }

  // switch (ping.gameState)
  // {
  //   case GameState::CALIBRATION:
  //     ping.calibrate();
  //     return;
  //   case GameState::MATCH_INIT:
  //     ping.initMatch();
  //     return;
  //   case GameState::MATCH_SERVE:
  //     ping.serveMatch();
  //     return;
  //   case GameState::SERVE_PROGRESS:
  //     ping.serveProgress();
  //     return;
  //   case GameState::MATCH_RUN:
  //     ping.runMatch();
  //     return;
  //   case GameState::BOUNCE_PROGRESS:
  //     ping.bounceProgess();
  //     return;
  //   case GameState::CENTER_PROGRESS:
  //     ping.centerProgress();
  //     return;
  //   case GameState::MATCH_END:
  //     ping.endMatch();
  //     return;
  //   default:
  //     return;
  // }
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
