#include "Paddle.h"
#include "AccelStepper.h"
#include "Ball.h"
#include "utils.h"

// Paddle paddleL = Paddle();
// AccelStepper steppe;
byte last = 1;
AccelStepper stepperX;
AccelStepper stepperY;
Ball ball;

void setup()
{
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

  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);

    stepperX = AccelStepper(1, 4, 5);
    stepperX.setCurrentPosition(0);
    stepperX.setMaxSpeed(SPEED);
    stepperX.setAcceleration(ACCELERATION);

    stepperY = AccelStepper(1, 6, 7);
    stepperX.setCurrentPosition(0);
    stepperY.setMaxSpeed(SPEED);
    stepperY.setAcceleration(ACCELERATION);

  ball.setMotors(&stepperX,&stepperY);
  pinMode(32,INPUT_PULLUP);
  pinMode(34,INPUT_PULLUP);

  attachInterrupt(
      digitalPinToInterrupt(32), []()
      { 
        ball.calibrationState[0] = true;
    },
      LOW);

      attachInterrupt(
      digitalPinToInterrupt(34), []()
      { 
        ball.calibrationState[1] = true;
    },
      LOW);

  ball.calibrate();
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
  // Serial.println(digitalRead(32));
  // delay(1000);
}