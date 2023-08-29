#include "PaddleController.h"

PaddleController paddleL = PaddleController();

void setup()
{
  if (Serial.available())
  {
    Serial.begin(9600);
    Serial.println("Arduino DUE - PING");
  }

  paddleL.initializeEncoder(52, 53);
  paddleL.initializeMotor(2, 3);

  attachInterrupt(
      digitalPinToInterrupt(52), []()
      { paddleL.isrA(); },
      CHANGE);
  attachInterrupt(
      digitalPinToInterrupt(53), []()
      { paddleL.isrB(); },
      CHANGE);
}

void loop()
{
  paddleL.run();
}