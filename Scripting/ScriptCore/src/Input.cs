﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stulu {
	public static class Input {
		public static bool isKeyDown(KeyCode keycode) => InternalCalls.input_keyDown((uint)keycode);
		public static bool isMouseDown(MouseCode button) => InternalCalls.input_mouseDown((uint)button);
		public static bool isMouseDown(MouseButton button) => InternalCalls.input_mouseDown((uint)button);
		public static void setCursorMode(CursorMode mode) => InternalCalls.input_setCursorMode((uint)mode);
		public static CursorMode getCursorMode() => (CursorMode)InternalCalls.input_getCursorMode();
		public static Vector2 getMousePos() { Vector2 val; InternalCalls.input_getMousePos(out val); return val; }
		public static Vector2 getMouseDelta() { Vector2 val; InternalCalls.input_getMouseDelta(out val); return val; }
		public static float getAxis(KeyCode positive, KeyCode negative) {
			return (Input.isKeyDown(positive) ? 1f : 0f) - (Input.isKeyDown(negative) ? 1f : 0f);
		}
		public static Vector2 getAxis(KeyCode xPositive, KeyCode xNegative, KeyCode yPositive, KeyCode yNegative) {
			return new Vector2(getAxis(xPositive, xNegative), getAxis(yPositive, yNegative));
		}
		public static Vector3 getAxis(KeyCode xPositive, KeyCode xNegative, KeyCode yPositive, KeyCode yNegative,KeyCode zPositive, KeyCode zNegative) {
			return new Vector3(getAxis(xPositive, xNegative), getAxis(yPositive, yNegative), getAxis(zPositive, zNegative));
		}
	}
	public enum CursorMode { Normal = 0, Hidden = 1, Disabled = 2 };

	public enum MouseCode {
		Button1 = 0,
		Button2 = 1,
		Button3 = 2,
		Button4 = 3,
		Button5 = 4,
		Button6 = 5,
		Button7 = 6,
		Button8 = 7,
	};
	public enum MouseButton {
		Left = 0,
		Right = 1,
		Middle = 2,
		Last = 7,
	};
	public enum KeyCode {
		NONE = 0,
		Space = 32,
		Apostroph = 39, /* ' */
		Comma = 44, /* , */
		Minus = 45, /* - */
		Period = 46, /* . */
		Slash = 47, /* / */
		D0 = 48,
		D1 = 49,
		D2 = 50,
		D3 = 51,
		D4 = 52,
		D5 = 53,
		D6 = 54,
		D7 = 55,
		D8 = 56,
		D9 = 57,
		Semicolon = 59, /* ; */
		Equal = 61, /* = */

		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,

		LeftBracket = 91, /* [ */
		Backslash = 92, /* \ */
		RightBracket = 93, /* ] */
		GraveAccent = 96, /* ` */
		World1 = 161, /* non-US #1 */
		World2 = 162, /* non-US #2 */

		/* Function keys */
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		ScrollDown = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,

		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,

		KP0 = 320,
		KP1 = 321,
		KP2 = 322,
		KP3 = 323,
		KP4 = 324,
		KP5 = 325,
		KP6 = 326,
		KP7 = 327,
		KP8 = 328,
		KP9 = 329,

		KPDecimal = 330,
		KPDivide = 331,
		KPMultiply = 332,
		KPSubtract = 333,
		KPAdd = 334,
		KPEnter = 335,
		KPEqual = 336,

		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuber = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348,
	};
}