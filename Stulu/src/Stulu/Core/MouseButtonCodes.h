#pragma once
//by glfw3.h
namespace Stulu::Mouse {
	using MouseCodes = uint32_t;

	enum : MouseCodes {
		Button1         = 0,
		Button2         = 1,
		Button3         = 2,
		Button4         = 3,
		Button5         = 4,
		Button6         = 5,
		Button7         = 6,
		Button8         = 7,
		ButtonLast      = 7,
		ButtonLeft      = 0,
		ButtonRight     = 1,
		ButtonMiddle    = 2,
	};
}
#define MOUSE_BUTTON_1         0
#define MOUSE_BUTTON_2         1
#define MOUSE_BUTTON_3         2
#define MOUSE_BUTTON_4         3
#define MOUSE_BUTTON_5         4
#define MOUSE_BUTTON_6         5
#define MOUSE_BUTTON_7         6
#define MOUSE_BUTTON_8         7
#define MOUSE_BUTTON_LAST      MOUSE_BUTTON_8
#define MOUSE_BUTTON_LEFT      MOUSE_BUTTON_1
#define MOUSE_BUTTON_RIGHT     MOUSE_BUTTON_2
#define MOUSE_BUTTON_MIDDLE    MOUSE_BUTTON_3