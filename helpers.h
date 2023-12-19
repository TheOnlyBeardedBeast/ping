#define CHECK_BIT(var,pos) ((var) & (1<<(pos))) ? HIGH : LOW

typedef void (*VoidCallback)();
typedef bool (*BoolCallback)();