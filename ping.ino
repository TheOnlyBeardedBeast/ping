// #include "Paddle.h"
#include "AccelStepper.h"
// #include "Ball.h"
#include "utils.h"
#include "DueTimer.h"
#include "Ping.h"

// Paddle paddleL = Paddle();
// AccelStepper steppe;
byte last = 1;
AccelStepper stepperX;
AccelStepper stepperY;
Ball ball;
Ping ping;

void setup()
{
  Serial.begin(9600);
  Serial.println("Arduino DUE - PING");

  randomSeed(analogRead(0));

  // paddleL.initializeEncoder(52, 53);
  // paddleL.initializeStepper(2, 3);

  // attachInterrupt(
  //     digitalPinToInterrupt(52), []()
  //     { paddleL.isrA(); },
  //     CHANGE);
  // attachInterrupt(
  //     digitalPinToInterrupt(53), []()
  //     { paddleL.isrB(); },
  //     CHANGE);

  // ball setup

  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);

  stepperX = AccelStepper(1, 6, 7);
  // stepperX.setCurrentPosition(0);
  // stepperX.setMaxSpeed(SPEED);
  // stepperX.setAcceleration(ACCELERATION);

  stepperY = AccelStepper(1, 8, 9);
  // stepperX.setCurrentPosition(0);
  // stepperY.setMaxSpeed(SPEED);
  // stepperY.setAcceleration(ACCELERATION);

  ball.setMotors(&stepperX, &stepperY);

  pinMode(22, INPUT_PULLUP);
  pinMode(23, INPUT_PULLUP);
  pinMode(24, INPUT_PULLUP);
  pinMode(25, INPUT_PULLUP);

  ping.init(&ball);

  // attachInterrupt(
  //     digitalPinToInterrupt(22), []()
  //     { ball.calibrationState[0] = true; },
  //     LOW);

  // attachInterrupt(
  //     digitalPinToInterrupt(23), []()
  //     { ball.calibrationState[1] = true; },
  //     LOW);

  // attachInterrupt(
  //     digitalPinToInterrupt(24), []()
  //     { ball.calibrationState[2] = true; },
  //     LOW);
  
  // attachInterrupt(
  //     digitalPinToInterrupt(25), []()
  //     { ball.calibrationState[3] = true; },
  //     LOW);

  // ball.calibrate();
  // Timer.getAvailable().attachInterrupt([](){
  //   ball.run();
  // }).start(10);

  // ball.calibrate();
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