#if !defined(MOUSE_H_)
#define MOUSE_H_

enum MouseButtonType
{
	MOUSE_LEFT,
	MOUSE_MIDDLE, // TODO(denis): add this functionality
	MOUSE_RIGHT,
	
	MOUSE_BUTTON_COUNT
};

struct MouseButton
{
	bool pressed;
	bool wasPressed;
	v2f startPos;
};

struct Mouse
{
    v2f pos;
	MouseButton buttons[3];
};

enum MouseButtonState
{
	MOUSE_INACTIVE,
	MOUSE_START_PRESS,
	MOUSE_PRESSED,
	MOUSE_RELEASED
};

static inline MouseButtonState getMouseButtonState(Mouse* mouse, MouseButtonType buttonType)
{
	ASSERT(mouse);
	
	if (mouse->buttons[buttonType].pressed && !mouse->buttons[buttonType].wasPressed)
	{
		return MOUSE_START_PRESS;
	}
	else if (mouse->buttons[buttonType].pressed && mouse->buttons[buttonType].wasPressed)
	{
		return MOUSE_PRESSED;
	}
	else if (!mouse->buttons[buttonType].pressed && mouse->buttons[buttonType].wasPressed)
	{
		return MOUSE_RELEASED;
	}
	
	return MOUSE_INACTIVE;
}

#endif