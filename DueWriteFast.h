#pragma once

typedef byte PinStatus;
typedef byte pin_size_t;
typedef Pio GPIO_TypeDef;

static inline void digitalWriteFast(pin_size_t pin, PinStatus val) __attribute__((always_inline, unused));
static inline void digitalWriteFast(pin_size_t pin, PinStatus val) {
//   PinName pin_name = g_APinDescription[pin].name;
//   uint16_t mask = mask_table[pin_name & 0xf];
//   GPIO_TypeDef  * const port = port_table[pin_name >> 4];
//   if (val) port->BSRR = mask;
//   else port->BSRR = (uint32_t)(mask << 16);
    // digitalPinToPort(pin)->PIO_ODSR = d;
    digitalWrite(pin,val);
}

static inline void digitalToggleMask(uint16_t mask,Pio  *port) __attribute__((always_inline, unused));
static inline void digitalToggleMask(uint16_t mask,Pio  *port) {
    port->PIO_ODSR ^= mask;
}


static inline uint32_t getPinMask(pin_size_t pin) __attribute__((always_inline, unused));
static inline uint32_t getPinMask(pin_size_t pin) {
//   PinName pin_name = g_APinDescription[pin].name;
//   uint16_t mask = mask_table[pin_name & 0xf];
//   GPIO_TypeDef  * const port = port_table[pin_name >> 4];
//   if (val) port->BSRR = mask;
//   else port->BSRR = (uint32_t)(mask << 16);
    // digitalPinToPort(pin)->PIO_ODSR = d;
    return digitalPinToBitMask(pin);
}

static inline Pio* getPinPort(pin_size_t pin) __attribute__((always_inline, unused));
static inline Pio* getPinPort(pin_size_t pin) {
//   PinName pin_name = g_APinDescription[pin].name;
//   uint16_t mask = mask_table[pin_name & 0xf];
//   GPIO_TypeDef  * const port = port_table[pin_name >> 4];
//   if (val) port->BSRR = mask;
//   else port->BSRR = (uint32_t)(mask << 16);
    // digitalPinToPort(pin)->PIO_ODSR = d;
    return digitalPinToPort(pin);
}
