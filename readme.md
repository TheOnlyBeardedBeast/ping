# Ping

is a an electromechanical tabble tennis game like the Atari pong table.

WORK IN PROGRESS

## About

This is a 2 player game, each player controls a paddle, the paddles are passing a ball between each other. If a player misses to catch the ball, the other player gets a point.

Each paddle is controlled with a nema17 motor, and a rotary encoder sets the speed and the direction of the paddle. The ball is controlled with 2 nema17 motors so it can move on 2 axes.

## Parts

- 4 x Nema17 Stepper motor
- 2 x E38S6G5-600B-G24N Rotary encoder
- 1 x CNC shield V3.0
- 4 x TMC2209 stepper motor driver
- 1 x Arduino due (probably will upgrade to Giga)
- 1 X 24V 16A PSU (120W)

## Progress

Currently the paddle implementation is done and working.

## Connections

### Axis motors

It is really important that the ball motors are on inputs which are based on the same port, this way we can modify the arduino output in the same time for both motors.

- 22 (PB_6) A motor step
- 23 (PB_7) A motor dir
- 24 (PB_8) B motor step
- 25 (PB_9) B motor dir

#### Limit switches
- 32
- 33
- 34
- 35
- 36
- 37
- 38
- 39
- 40

use
https://github.com/mikaelpatel/Arduino-Scheduler

