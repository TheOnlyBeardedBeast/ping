#define CHECK_BIT(var,pos) ((var) & (1<<(pos))) ? HIGH : LOW

typedef void (*VoidCallback)();
#if defined(ARDUINO_GIGA)
    typedef bool (*BoolCallback)();
#elif defined(ARDUINO_SAM_DUE)
    typedef int (*BoolCallback)();
#endif
