#pragma once

#include <Queue.h>
#include <DueTC.h>

#include "../DueWriteFast.h"
#include "../utils.h"
#include "../Models/Directable.h"

#include "Timing.h"
#include "MotionUtils.h"

class PongStepper
{
private:
    size_t step_pin;
    size_t dir_pin;

public:
    void init(size_t _step_pin, size_t _dir_pin, size_t _timerId);

private:
    uint32_t acc;
    uint32_t acc_pw;
    uint32_t acc_2;

    uint32_t targetSpeed;
    uint32_t targetSpeed_pw;
    uint32_t targetSpeed_2;

    uint32_t speed;
    uint32_t delayPeriod;

public:
    void setAcceleration(uint32_t _acceleration);
    void setTargetSpeed(uint32_t _targetSpeed);

private:
    uint16_t position;
    uint16_t limit;
    StepDirection dir = StepDirection::NONE;

    uint16_t distance;
    uint16_t distanceRun;

    uint16_t accSteps;
    uint16_t deccSteps;

public:
    uint16_t getPosition();

    void setLimit(uint16_t _limit);
    void setPosition(uint16_t _position);

    void step();
    void stepTo(uint16_t _position);
    void stepTo(uint16_t _position, uint32_t _speed);
    void stop();

private:
    bool _isMoving = false;
    bool _isStopping = false;

    bool isRunning();

public:
    bool isMoving();
    bool isStopping();
}