#include "Paddle.h"
#include "FastAccelStepper.h"
#include "Ball.h"
#include "utils.h"

// Paddle paddleL = Paddle();
// AccelStepper steppe;
byte last = 1;
FastAccelStepperEngine engine = FastAccelStepperEngine();
FastAccelStepper *stepperX;
FastAccelStepper *stepperY;
Ball ball;

void setup()
{
  engine.init();
  Serial.begin(9600);
  Serial.println("Arduino DUE - PING");

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

  stepperX = engine.stepperConnectToPin(6);
  stepperX->setDirectionPin(7);
  int speed = 12000;
  stepperX->setSpeedInHz(speed);
  stepperX->setAcceleration(speed>>2);
  stepperX->setDelayToEnable(1000);
  // stepperX.setCurrentPosition(0);
  // stepperX.setMaxSpeed(SPEED);
  // stepperX.setAcceleration(ACCELERATION);
  stepperY = engine.stepperConnectToPin(8);
  stepperY->setDirectionPin(9);
  
  // stepperX.setCurrentPosition(0);
  // stepperY.setMaxSpeed(SPEED);
  // stepperY.setAcceleration(ACCELERATION);

  // ball.setMotors(stepperX, stepperY);

  pinMode(22, INPUT_PULLUP);
  pinMode(23, INPUT_PULLUP);
  pinMode(24, INPUT_PULLUP);
  pinMode(25, INPUT_PULLUP);

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
  stepperX->moveTo(10000<<4);
}

// void loop()
// {
//   if(stepperX.distanceToGo() == 0 && stepperY.distanceToGo() == 0){
//     switch (last)
//     {
//     case 0:
//       ball.setposition(0,200);
//       last = 1;
//       break;
//     case 1:
//       ball.setposition(200,200);
//       last = 2;
//       break;
//     case 2:
//       ball.setposition(200,0);
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
}