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

#define KEY_TAB 0x09
#define KEY_ENTER 0x0D
#define KEY_SPACEBAR 0x20
#define KEY_LEFT_ARROW 0x25
#define KEY_UP_ARROW 0x26
#define KEY_RIGHT_ARROW 0x27
#define KEY_DOWN_ARROW 0x28
#define KEY_F1 0x70
#define KEY_F2 0x71
#define KEY_F3 0x72
#define KEY_F4 0x73
#define KEY_F5 0x74
#define KEY_F6 0x75
#define KEY_F7 0x76
#define KEY_F8 0x77
#define KEY_F9 0x78
#define KEY_F10 0x79
#define KEY_F11 0x7A
#define KEY_F12 0x7B
#define KEY_ESC 0x1B
#define KEY_TILDE 0xC0

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