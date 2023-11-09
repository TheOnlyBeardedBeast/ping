// #include "Paddle.h"
#include "AccelStepper.h"
// #include "Ball.h"
// #include "utils.h"
// #include "Ping.h"
#include <Arduino.h>
#include "XYS.h"
int POSITION = 16000;

// Paddle paddleL = Paddle();
// AccelStepper steppe;
int prev = 0;
int last = 0;
AccelStepper stepperX;
// AccelStepper steppery;
// AccelStepper stepperY;
// Ball ball;
// Ping ping;

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
  last++;
}

int _period = 5000;

void setup()
{
  // Serial.begin(115200);
  // while (!Serial) ;
  // stepper_drivers.setup(Serial1);

  pinMode(22,OUTPUT);
  pinMode(23,OUTPUT);
  pinMode(24,OUTPUT);
  pinMode(25,OUTPUT);
  // pinMode(LED_BUILTIN,OUTPUT);


  // digitalWrite(LED_BUILTIN,HIGH);

  // Serial.println("Arduino DUE - PING");
  
  // pinMode(D4,OUTPUT);
  // pinMode(D5,OUTPUT);

  // xyTimer.attachInterrupt(1,ballTimerIsr);
  // stepperX = AccelStepper(1, 22, 23);
  // steppery = AccelStepper(1, 24, 25);
  // stepperX.setMinPulseWidth(5);

  // steppery.moveTo(400000);
  // steppery.setMaxSpeed(20000);
  // steppery.setAcceleration(5*12000);

  // stepperX.setMaxSpeed(16000);
  // stepperX.setAcceleration(160000);
  // stepperX.moveTo(400000);
  
  xyStepper.init(22,23,24,25);
  xyStepper.setTimer(&xyTimer);
  xyStepper.setCallback(ballTimerIsr);
  delay(2000);
  xyStepper.setPosition(POSITION,0);
  // xyStepper.printBresenham();
  // xyStepper.printRamp();
}


// void setupGame()
// {
//   Serial.begin(9600);
//   Serial.println("Arduino DUE - PING");

//   randomSeed(analogRead(0));

//   // paddleL.initializeEncoder(52, 53);
//   // paddleL.initializeStepper(2, 3);

//   // attachInterrupt(
//   //     digitalPinToInterrupt(52), []()
//   //     { paddleL.isrA(); },
//   //     CHANGE);
//   // attachInterrupt(
//   //     digitalPinToInterrupt(53), []()
//   //     { paddleL.isrB(); },
//   //     CHANGE);

//   // ball setup

//   pinMode(6, OUTPUT);
//   pinMode(7, OUTPUT);
//   pinMode(8, OUTPUT);
//   pinMode(9, OUTPUT);

//   stepperX = AccelStepper(1, 6, 7);
//   // stepperX.setCurrentPosition(0);
//   // stepperX.setMaxSpeed(SPEED);
//   // stepperX.setAcceleration(ACCELERATION);

//   stepperY = AccelStepper(1, 8, 9);
//   // stepperX.setCurrentPosition(0);
//   // stepperY.setMaxSpeed(SPEED);
//   // stepperY.setAcceleration(ACCELERATION);

//   ball.setMotors(&stepperX, &stepperY);

//   pinMode(22, INPUT_PULLUP);
//   pinMode(23, INPUT_PULLUP);
//   pinMode(24, INPUT_PULLUP);
//   pinMode(25, INPUT_PULLUP);

//   ping.init(&ball);

//   // attachInterrupt(
//   //     digitalPinToInterrupt(22), []()
//   //     { ball.calibrationState[0] = true; },
//   //     LOW);

//   // attachInterrupt(
//   //     digitalPinToInterrupt(23), []()
//   //     { ball.calibrationState[1] = true; },
//   //     LOW);

//   // attachInterrupt(
//   //     digitalPinToInterrupt(24), []()
//   //     { ball.calibrationState[2] = true; },
//   //     LOW);
  
//   // attachInterrupt(
//   //     digitalPinToInterrupt(25), []()
//   //     { ball.calibrationState[3] = true; },
//   //     LOW);

//   // ball.calibrate();
//   // Timer.getAvailable().attachInterrupt([](){
//   //   ball.run();
//   // }).start(10);

//   // ball.calibrate();
// }

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

// void loopGame()
// {
//   switch (ping.gameState)
//   {
//   case GameState::CALIBRATION:
//     ping.calibrate();
//     return;
//   case GameState::MATCH_INIT:
//     ping.initMatch();
//     return;
//   case GameState::MATCH_SERVE:
//     ping.serveMatch();
//     return;
//   case GameState::MATCH_RUN:
//     ping.runMatch();
//     return;
//   default:
//     return;
//   }
// }

void loop() {
  // digitalWrite(23,HIGH);
  // digitalWrite(22,HIGH);
  // delayMicroseconds(3);
  // digitalWrite(22,LOW);
  // delayMicroseconds(1000);
  // stepperX.run();
  // delayMicroseconds(40);
  if(!xyStepper.moving)
  {
    
    POSITION = POSITION * -1;
    if(POSITION < 0)
    {
      xyStepper.setPosition(0,0);
    }
    // xyStepper.printBresenham();
    // xyStepper.printRamp();
  }
  // driver.step();
  // delay(1000);
  // digitalWrite(23, HIGH);
  // digitalWrite(22,HIGH);
  // delayMicroseconds(1);
  // digitalWrite(22,LOW);
  // delayMicroseconds(_period);

  // if(_period > 40)
  // {
  //   _period-=1;
  // }
  // stepperX.run();
  // steppery.run();
    // if(prev!=last)
    // {
    //   // Serial.print("delay:");
    //   //   Serial.println(xyStepper.delayPeriod);
    //   //   Serial.print("multiplier:");
    //   //   Serial.println(xyStepper.multiplier);
    //   //   Serial.print("accelD:");
    //   //   Serial.println(xyStepper.accelDistance);
    //   if(prev%160==0){
    //     Serial.print("y:");
    //     Serial.println(xyStepper.delayPeriod);
    //   }
    //   prev = last;
    // }
    // if(last==1000 && prev != last){
    //   Serial.print("y:");
    //     Serial.println(xyStepper.delayPeriod);
    //   // Serial.print("X:");
    //   // Serial.print(xyStepper.getX());
    //   // Serial.print("y:");
    //   // Serial.print(xyStepper.getY());
    //   // Serial.println();
    // }
}