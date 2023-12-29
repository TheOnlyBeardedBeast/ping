#pragma once

typedef byte PinStatus;
typedef byte pin_size_t;
typedef Pio GPIO_TypeDef;

static inline void digitalWriteFast(pin_size_t pin, PinStatus val) __attribute__((always_inline, unused));
static inline void digitalWriteFast(pin_size_t pin, PinStatus val) {
    // Pio *port = digitalPinToPort(pin);
    // uint32_t mask = digitalPinToBitMask(pin);

    // bitWrite(port->PIO_ODSR,mask,val);
    Serial.println("WRITE");
    Serial.println(pin);
    Serial.println(val);
    digitalWrite(pin,val);
}

static inline void digitalToggleMask(uint16_t mask,Pio  *port) __attribute__((always_inline, unused));
static inline void digitalToggleMask(uint16_t mask,Pio  *port) {
    port->PIO_ODSR ^= mask;
}


static inline uint32_t getPinMask(pin_size_t pin) __attribute__((always_inline, unused));
static inline uint32_t getPinMask(pin_size_t pin) {
    return digitalPinToBitMask(pin);
}

static inline Pio* getPinPort(pin_size_t pin) __attribute__((always_inline, unused));
static inline Pio* getPinPort(pin_size_t pin) {
    return digitalPinToPort(pin);
}
