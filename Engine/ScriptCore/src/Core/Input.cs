#pragma warning disable 1591
namespace Stulu {
	/// <summary>
	/// Get Input data from the engine
	/// </summary>
	public static class Input {
		/// <summary>
		/// True during the frame the user starts pressing down the key
		/// </summary>
		public static bool getKeyDown(KeyCode keycode) => InternalCalls.input_getKeyDown((uint)keycode);
		/// <summary>
		/// True during the frame the user starts pressing down the key
		/// </summary>
		public static bool getKeyDown(int keycode) => InternalCalls.input_getKeyDown((uint)keycode);
		/// <summary>
		/// True during the frame the user releases the key
		/// </summary>
		public static bool getKeyUp(KeyCode keycode) => InternalCalls.input_getKeyUp((uint)keycode);
		/// <summary>
		/// True during the frame the user releases the key
		/// </summary>
		public static bool getKeyUp(int keycode) => InternalCalls.input_getKeyUp((uint)keycode);
		/// <summary>
		/// True if the key is pressed
		/// </summary>
		public static bool getKey(KeyCode keycode) => InternalCalls.input_getKey((uint)keycode);
		/// <summary>
		/// True if the key is pressed
		/// </summary>
		public static bool getKey(int keycode) => InternalCalls.input_getKey((uint)keycode);
		/// <summary>
		/// True during the frame the user starts pressing down the button
		/// </summary>
		public static bool getMouseButtonDown(MouseButton button) => InternalCalls.input_getMouseDown((uint)button);
		/// <summary>
		/// True during the frame the user starts pressing down the button
		/// </summary>
		public static bool getMouseButtonDown(int button) => InternalCalls.input_getMouseDown((uint)button);
		/// <summary>
		/// True during the frame the user releases the button
		/// </summary>
		public static bool getMouseButtonUp(MouseButton button) => InternalCalls.input_getMouseUp((uint)button);
		/// <summary>
		/// True during the frame the user releases the button
		/// </summary>
		public static bool getMouseButtonUp(int button) => InternalCalls.input_getMouseUp((uint)button);
		/// <summary>
		/// True if the button is pressed
		/// </summary>
		public static bool getMouseButton(MouseButton button) => InternalCalls.input_getMouse((uint)button);
		/// <summary>
		/// True if the button is pressed
		/// </summary>
		public static bool getMouseButton(int button) => InternalCalls.input_getMouse((uint)button);
		/// <summary>
		/// Sets the Cursor mode
		/// </summary>
		public static void setCursorMode(CursorMode mode) => InternalCalls.input_setCursorMode((uint)mode);
		/// <summary>
		/// Return the current Cursor mode
		/// </summary>
		public static CursorMode getCursorMode() => (CursorMode)InternalCalls.input_getCursorMode();
		/// <summary>
		/// Gets the mouse position
		/// </summary>
		public static Vector2 getMousePos() { Vector2 val; InternalCalls.input_getMousePos(out val); return val; }
		/// <summary>
		/// Gets the mouse movement in this frame
		/// </summary>
		public static Vector2 getMouseDelta() { Vector2 val; InternalCalls.input_getMouseDelta(out val); return val; }
		/// <summary>
		/// Gets a 1D axis between two keys
		/// </summary>
		public static float getAxis(KeyCode positive, KeyCode negative) {
			return (Input.getKey(positive) ? 1f : 0f) - (Input.getKey(negative) ? 1f : 0f);
		}
		/// <summary>
		/// Gets a 2D axis between two keys for each direction
		/// </summary>
		public static Vector2 getAxis(KeyCode xPositive, KeyCode xNegative, KeyCode yPositive, KeyCode yNegative) {
			return new Vector2(getAxis(xPositive, xNegative), getAxis(yPositive, yNegative));
		}
		/// <summary>
		/// Gets a 3D axis between two keys for each direction
		/// </summary>
		public static Vector3 getAxis(KeyCode xPositive, KeyCode xNegative, KeyCode yPositive, KeyCode yNegative,KeyCode zPositive, KeyCode zNegative) {
			return new Vector3(getAxis(xPositive, xNegative), getAxis(yPositive, yNegative), getAxis(zPositive, zNegative));
		}
	}
	/// <summary>
	/// How the cursor behaves
	/// </summary>
	public enum CursorMode { 
		/// <summary>
		/// Normal Cursor
		/// </summary>
		Normal = 0, 
		/// <summary>
		/// Only Hidden but can interacts with other stuff
		/// </summary>
		Hidden = 1, 
		/// <summary>
		/// Hidden and disabled, but you can still get the movement
		/// </summary>
		Disabled = 2 
	};
	/// <summary>
	/// Defines values that specify the buttons on a mouse device
	/// </summary>
	public enum MouseButton {
		/// <summary>
		/// Left Mouse button
		/// </summary>
		Button1 = 0,
		/// <summary>
		/// Right Mouse button
		/// </summary>
		Button2 = 1,
		/// <summary>
		/// Middle Mouse button
		/// </summary>
		Button3 = 2,
		/// <summary>
		/// The side button in the back
		/// </summary>
		Button4 = 3,
		/// <summary>
		/// The side button in the front
		/// </summary>
		Button5 = 4,

		/// <summary>
		/// Left Mouse button
		/// </summary>
		Left = Button1,
		/// <summary>
		/// Right Mouse button
		/// </summary>
		Right = Button2,
		/// <summary>
		/// Middle Mouse button
		/// </summary>
		Middle = Button3,
		/// <summary>
		/// The side button in the back
		/// </summary>
		X1 = Button4,
		/// <summary>
		/// The side button in the back
		/// </summary>
		SideButtonBack = Button4,
		/// <summary>
		/// The side button in the front
		/// </summary>
		X2 = Button5,
		/// <summary>
		/// The side button in the front
		/// </summary>
		SideButtonFront = Button5,
	};
	/// <summary>
	/// Defines values that specify the keys on a keyboard device
	/// </summary>
	public enum KeyCode {
		NONE = 0,
		Space = 32,
		/// <summary>
		/// '
		/// </summary>
		Apostroph = 39,
		/// <summary>
		/// ,
		/// </summary>
		Comma = 44,
		/// <summary>
		/// -
		/// </summary>
		Minus = 45,
		/// <summary>
		/// .
		/// </summary>
		Period = 46,
		/// <summary>
		/// /
		/// </summary>
		Slash = 47,
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
		/// <summary>
		/// ;
		/// </summary>
		Semicolon = 59,
		/// <summary>
		/// =
		/// </summary>
		Equal = 61,

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
		/// <summary>
		/// [
		/// </summary>
		LeftBracket = 91,
		/// <summary>
		/// \
		/// </summary>
		Backslash = 92,
		/// <summary>
		/// ]
		/// </summary>
		RightBracket = 93,
		/// <summary>
		/// `
		/// </summary>
		GraveAccent = 96,
		/// <summary>
		/// non-US #1
		/// </summary>
		World1 = 161,
		/// <summary>
		/// non-US #2
		/// </summary>
		World2 = 162,

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
