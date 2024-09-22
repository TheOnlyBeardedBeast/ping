function calculateRequiredSpeed(acceleration: number, totalTime: number, distance: number): number {
    // Calculate the time needed to reach max speed
    const t_acc = Math.sqrt(distance / (2 * acceleration)) - 400/acceleration;
    
    // Total time for acceleration and deceleration
    const totalAccelDecelTime = 2 * t_acc;
    
    // Time for constant speed
    const t_constant = totalTime - totalAccelDecelTime;
    
    // Distance covered during acceleration and deceleration
    const d_acc = 0.5 * acceleration * t_acc * t_acc;
    
    // Remaining distance at constant speed
    const d_constant = distance - 2 * d_acc;
    
    // Calculate the required max speed
    const v_max = d_constant / t_constant;
    
    return v_max;
}

// Example usage
const acceleration = 20000;    // Acceleration in steps/s²
const totalTime = 1;           // Total time in seconds
const distance = 1000;        // Total distance in steps

const requiredSpeed = calculateRequiredSpeed(acceleration, totalTime, distance);

console.log(`Required maximum speed: ${requiredSpeed.toFixed(2)} steps/s`);
