#define PI 3.14159265358979323846
#define HALF_PI 1.57079632679
#define SPEED 2000
#define ACCELERATION 200
#define CALIBRATION_SPEED 800
#define SAFEZONE_WIDTH 50
#define SERVE_PIN(player) (player == 0 ? 42 : 43)
#define PADDLE_WIDTH 0

// static inline void delayNanoseconds(uint32_t) __attribute__((always_inline, unused));
// static inline void delayNanoseconds(uint32_t nsec){
//     /*
//      * Based on Paul Stoffregen's implementation
//      * for Teensy 3.0 (http://www.pjrc.com/)
//      */
//     if (nsec == 0) return;
//     uint32_t n = (nsec * 1000) / 35714;
//     asm volatile(
//         "L_%=_delayNanos:"       "\n\t"
//         "subs   %0, #1"                 "\n\t"
//         "bne    L_%=_delayNanos" "\n"
//         : "+r" (n) :
//     );
// }

// __asm__ __volatile__(
//   ".macro NOPX      P1          \n\t"
//   ".rept &P1                    \n\t"
//   "NOP                          \n\t"
//   ".endr                        \n\t"   // End of Repeat
//   ".endm                        \n\t"   // End of macro
// );
// void setup() {

//   __asm__ __volatile__("NOPX 82");  
 
//   // 101 ticks = 1202 ns 
// }

// void loop() {
// }