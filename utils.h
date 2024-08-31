#pragma once

#define PI 3.14159265358979323846
#define HALF_PI 1.57079632679
// TMC - 20k+
// A4988 - 4k
// ATARI PONG ANALYSIS
// GAME SPEEDS
// 0-----0.3175m/s - 635step/s
// 1-----0.4978m/s - 995stes/s
// 2-----0.6782m/s - 1,357step/s
// 3-----0.8585m/s - 1,717step/s
// 4-----1.0389m/s - 2,078step/s
// 5-----1.2192m/s - 2,440step/s
// 6-----1.5875m/s - 3,175step/s

#define SPEED 1750 // 0.875m/s
#define ACCELERATION 10 * SPEED
#define CALIBRATION_SPEED 250
#define SAFEZONE_WIDTH 10
#define SERVE_PIN(player) (player == 0 ? 42 : 43)
#define CALIBRATION_LENGTH 100000
/// 0 for trigonometry 1 for Bresenham
#define LINE 0
