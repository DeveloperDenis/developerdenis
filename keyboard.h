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

#define KEY_SPACEBAR 0x20
#define KEY_LEFT_ARROW 0x25
#define KEY_UP_ARROW 0x26
#define KEY_RIGHT_ARROW 0x27
#define KEY_DOWN_ARROW 0x28
#define KEY_F1 0x70

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