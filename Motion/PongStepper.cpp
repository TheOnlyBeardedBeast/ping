#include "PongStepper.h"

void PongStepper::init(size_t _step_pin, size_t _dir_pin, size_t _timerId)
{
    pinMode(_step_pin, OUTPUT);
    this->step_pin = _step_pin;
    pinMode(_dir_pin, OUTPUT);
    this->dir_pin = _dir_pin;

    this->timerId = _timerId;
}

void PongStepper::setPulseWidth(byte width)
{
    this->pulseWidth = width;
}

uint32_t PongStepper::getSpeed()
{
    return this->speed;
}

uint32_t PongStepper::getTargetSpeed()
{
    return this->targetSpeed;
}

uint32_t PongStepper::getAcceleration()
{
    return this->acc;
}

void PongStepper::setAcceleration(uint32_t _acceleration)
{
    this->acc = _acceleration;
    this->acc_pw = _acceleration * _acceleration;
    this->acc_2 = _acceleration << 1;
}

void PongStepper::setTargetSpeed(uint32_t _targetSpeed)
{
    this->targetSpeed = _targetSpeed;
    this->targetSpeed_pw = _targetSpeed * targetSpeed;
    this->targetSpeed_2 = _targetSpeed << 1;
}

uint16_t PongStepper::getPosition()
{
    return this->position;
}

void PongStepper::setLimit(uint16_t _limit)
{
    this->limit = _limit;
}

void PongStepper::setPosition(uint16_t _position)
{
    this->position = _position;
}

void PongStepper::setDirection(StepDirection dir)
{
    if (this->dir != dir)
    {
        this->dir = dir;
        digitalWriteFast(this->dir_pin, dirToPin(this->dir));
    }

    if (this->onDirectionChange != NULL)
    {
        this->onDirectionChange(dir);
    }
}

void PongStepper::_step()
{

    this->stepSingle();
    this->distanceRun++;

    if (this->distanceRun < this->accSteps)
    {
        this->speed = calculateAccelerationSpeed(this->speed * this->speed, START_SPEED, this->acc_2);
        this->delayPeriod = TICKS / this->speed;
    }
    else if (this->distanceRun > this->distance - this->deccSteps)
    {
        this->speed = calculateDeccelerationSpeed(this->speed * this->speed, START_SPEED, this->acc_2);
        this->delayPeriod = TICKS / this->speed;
    }
}

void PongStepper::step()
{
    if (!this->isRunning() || this->dir == StepDirection::NONE)
    {
        this->_isMoving = false;
        this->distanceRun = 0;
        this->distance = 0;
        // stop timer;

        return;
    }

    this->step();

    // change timer here
}

void PongStepper::stepSync()
{
    uint32_t now = micros();

    if (!this->isMoving() ||
        (this->isRunning() && now > this->nextStepAt))
    {
        return;
    }

    if (!this->isRunning() || this->dir == StepDirection::NONE)
    {
        this->_isMoving = false;
        this->distanceRun = 0;
        this->distance = 0;
        this->nextStepAt = 0;
        this->speed = 0;

        return;
    }

    this->_step();
    this->nextStepAt = now + this->delayPeriod;
}

void PongStepper::_stepTo(uint16_t _position)
{
    if (this->isStopping())
    {
        return;
    }

    int16_t diff = static_cast<uint16_t>(this->position) - static_cast<int16_t>(_position);
    StepDirection targetDirection = diffToDirection(diff);

    if (targetDirection == StepDirection::NONE)
    {
        return;
    }

    if (this->isRunning() && this->dir != targetDirection)
    {
        this->stop();
        return;
    }

    this->dir = targetDirection;
    this->distanceRun = 0;
    this->_isMoving = true;

    digitalWriteFast(this->dir_pin, dirToPin(targetDirection));

    this->distance = abs(diff);
    uint16_t halfDistance = this->distance >> 1;

    this->speed = START_SPEED;

    this->accSteps = calculateRampSteps(this->targetSpeed_pw, START_SPEED_PW, this->acc_2);
    this->deccSteps = this->accSteps;

    if (this->accSteps >= halfDistance)
    {
        this->accSteps = halfDistance;
        this->deccSteps = halfDistance - this->accSteps;
    }

    this->delayPeriod = TICKS / this->speed;
}

void PongStepper::_stepTo(uint16_t _position, uint32_t _speed)
{
    this->setTargetSpeed(_speed);
    this->_stepTo(_position);
}

void PongStepper::stepTo(uint16_t _position, uint32_t _speed)
{
    this->setTargetSpeed(_speed);
    this->stepTo(_position);
}

void PongStepper::stepToSync(uint16_t _position, uint32_t _speed)
{
    this->setTargetSpeed(_speed);
    this->stepToSync(_position);
}

void PongStepper::stepTo(uint16_t _position)
{
    this->_stepTo(_position);
    // start timer
}

void PongStepper::stepToSync(uint16_t _position)
{
    this->_stepTo(_position);
    this->nextStepAt = micros() + this->delayPeriod;
}

void PongStepper::stop()
{
    if (!this->isRunning() || this->isStopping())
    {
        return;
    }

    this->distance = this->distanceRun + calculateRampSteps(this->speed * this->speed, START_SPEED_PW, this->acc_2);
}

void PongStepper::stepSingle()
{
    digitalWriteFast(this->step_pin, HIGH);
    this->position += this->dir;

    if (this->onStepChange != NULL)
    {
        this->onStepChange();
    }
}

void PongStepper::stepSingleSync()
{
    digitalWriteFast(this->step_pin, HIGH);
    this->position += this->dir;

    delayMicroseconds(this->pulseWidth);

    clearStep();
}

bool PongStepper::isRunning()
{
    return this->distanceRun != this->distance;
}

bool PongStepper::isMoving()
{
    return this->isMoving;
}

bool PongStepper::isStopping()
{
    return this->isStopping;
}

void PongStepper::clearStep()
{
    digitalWriteFast(this->step_pin, LOW);
}
