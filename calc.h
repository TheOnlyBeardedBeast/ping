#include <Queue.h>
#include "utils.h"

struct MotionParams
{
    double maxSpeed;     // Maximum speed during the motion (steps/sec)
    double acceleration; // Acceleration (steps/sec^2)
};

struct MotionSegment
{
    long targetPosition;
    long distance;
    long targetVelocity;
    long initialVelocity;
};

double getTime(Queue<MotionSegment, 10> segments)
{
    double queueTime = 0;

    for (byte i = 0; i < segments.size; i++)
    {
        MotionSegment segment = segments.at(i);

        int accSteps = (segment.targetVelocity * segment.targetVelocity - segment.initialVelocity * segment.initialVelocity) / ACCELERATION;

        if (accSteps > segment.distance >> 1)
        {
            accSteps = segment.distance >> 1;
        }

        double achievedSpeed = segment.initialVelocity * segment.initialVelocity + 2 * ACCELERATION * accSteps;
        double cruiseDistance = segment.distance - 2 * accSteps;

        double accTime = (achievedSpeed - segment.initialVelocity) / ACCELERATION;
        double cruiseTime = cruiseDistance / achievedSpeed;

        queueTime += (2 * accTime) + cruiseTime;
    }

    return queueTime;
}

MotionParams calculateMotionParams(
    double distance, // Total distance in steps
    double t_accel,  // Time for acceleration phase (seconds)
    double t_const,  // Time for constant speed phase (seconds)
    double t_decel,  // Time for deceleration phase (seconds)
    double v0        // Initial velocity (steps/sec)
)
{
    // Step 1: Calculate the acceleration needed to reach v_max
    // v_max = v0 + a * t_accel
    // We need to solve for 'a' and 'v_max' under the distance and time constraints

    // Distance during acceleration (d_accel)
    // d_accel = v0 * t_accel + 0.5 * a * t_accel^2
    double accelTimeSquared = t_accel * t_accel;

    // Step 2: Distance covered during acceleration and deceleration
    // d_total = 2 * d_accel + d_const
    double t_total = t_accel + t_const + t_decel;

    // Calculate acceleration based on the constraints
    double a = (distance - v0 * t_total) / (t_accel * (t_accel + t_const));

    // Calculate the max speed reached during the constant speed phase
    double v_max = v0 + a * t_accel;

    // Return results
    return {v_max, a};
}

// interface MotionParams {
//     maxSpeed: number;      // Maximum speed during the motion (steps/sec)
//     acceleration: number;  // Acceleration (steps/sec^2)
// }

// function calculateMotionParams(
//     distance: number,      // Total distance in steps
//     t_accel: number,       // Time for acceleration phase (seconds)
//     t_const: number,       // Time for constant speed phase (seconds)
//     t_decel: number,       // Time for deceleration phase (seconds)
//     v0: number             // Initial velocity (steps/sec)
// ): MotionParams | null {
//     // Ensure that t_accel equals t_decel
//     if (t_accel !== t_decel) {
//         console.error("Error: Acceleration and deceleration times must be equal.");
//         return null;
//     }

//     // Step 1: Calculate the acceleration needed to reach v_max
//     // v_max = v0 + a * t_accel
//     // We need to solve for 'a' and 'v_max' under the distance and time constraints
//     // Step 2: Distance covered during acceleration and deceleration
//     // d_total = 2 * d_accel + d_const
//     const t_total = t_accel + t_const + t_decel;

//     // Initial guess for acceleration
//     let a = (distance - v0 * t_total) / (t_accel * (t_accel + t_const));

//     // Calculate the max speed reached during the constant speed phase
//     const v_max = v0 + a * t_accel;

//     // Step 3: Return results
//     return {
//         maxSpeed: v_max,
//         acceleration: a
//     };
// }

// // Example usage:
// const distance = 1200;  // Total distance in steps
// const t_accel = 0.075;  // Acceleration time in seconds
// const t_const = 0.871;  // Constant speed time in seconds
// const t_decel = 0.075;  // Deceleration time in seconds
// const v0 = 400;         // Initial velocity in steps/sec

// const result = calculateMotionParams(distance, t_accel, t_const, t_decel, v0);

// if (result) {
//     console.log(`Max Speed: ${result.maxSpeed.toFixed(2)} steps/sec`);
//     console.log(`Acceleration: ${result.acceleration.toFixed(2)} steps/sec²`);
// }
