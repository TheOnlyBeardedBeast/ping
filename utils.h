#pragma once

#define PI 3.14159265358979323846
#define HALF_PI 1.57079632679
// TMC - 20k+
// A4988 - 4k
#define SPEED 1200
#define ACCELERATION 10 * SPEED
#define CALIBRATION_SPEED 250
#define SAFEZONE_WIDTH 10
#define SERVE_PIN(player) (player == 0 ? 42 : 43)
#define CALIBRATION_LENGTH 100000
/// 0 for trigonometry 1 for Bresenham
#define LINE 0
