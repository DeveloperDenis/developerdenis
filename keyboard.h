#if !defined(KEYBOARD_H_)
#define KEYBOARD_H_

#define KEYBOARD_NUM_KEYS 256

struct Key
{
	bool pressed;
	bool changed;
};
struct Keyboard
{
	Key keys[KEYBOARD_NUM_KEYS];
};

enum
{
	KEY_TAB = 0x09,
	KEY_ENTER = 0x0D,
	KEY_SPACEBAR = 0x20,
	KEY_LEFT_ARROW = 0x25,
	KEY_UP_ARROW = 0x26,
	KEY_RIGHT_ARROW = 0x27,
	KEY_DOWN_ARROW = 0x28,
	KEY_A = 0x41,
	KEY_B = 0x42,
	KEY_C = 0x43,
	KEY_D = 0x44,
	KEY_S = 0x53,
	KEY_W = 0x57,
	KEY_F1 = 0x70,
	KEY_F2 = 0x71,
	KEY_F3 = 0x72,
	KEY_F4 = 0x73,
	KEY_F5 = 0x74,
	KEY_F6 = 0x75,
	KEY_F7 = 0x76,
	KEY_F8 = 0x77,
	KEY_F9 = 0x78,
	KEY_F10 = 0x79,
	KEY_F11 = 0x7A,
	KEY_F12 = 0x7B,
	KEY_ESC = 0x1B,
	KEY_TILDE = 0xC0
};

static inline bool isKeyDown(Keyboard keyboard, u32 keyCode)
{
	return keyboard.keys[keyCode].pressed;
}

static inline bool wasKeyDown(Keyboard keyboard, u32 keyCode)
{
	return !keyboard.keys[keyCode].pressed && keyboard.keys[keyCode].changed;
}

static inline bool isKeyUp(Keyboard keyboard, u32 keyCode)
{
	return !keyboard.keys[keyCode].pressed;
}

static inline bool wasKeyUp(Keyboard keyboard, u32 keyCode)
{
	return keyboard.keys[keyCode].pressed && keyboard.keys[keyCode].changed;
}

#endif