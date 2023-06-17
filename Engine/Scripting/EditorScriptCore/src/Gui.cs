using System.Runtime.CompilerServices;

namespace Stulu.Editor {
	/// <summary>
	/// You can use it draw windows in the Editor
	/// Is not available in the final build of the project
	/// </summary>
	public class Gui {
		/// <summary>
		/// Begins a new Window and returns false if collapsed, remeber to always call End()
		/// </summary>
		public static bool Begin(string header) => (InternalCalls.gui_Begin(header) == 1 ? true : false);
		/// <summary>
		/// Ends the current Window
		/// </summary>
		public static void End() => InternalCalls.gui_End();
		/// <summary>
		/// Creates a Text element
		/// </summary>
		public static void Text(string header) => InternalCalls.gui_Text(header);
		/// <summary>
		/// Creates a Checkbox, which returns true if checked
		/// </summary>
		public static bool CheckBox(string header, bool value) => InternalCalls.gui_CheckBox(header, value ? 1 : 0) == 0 ? false : true;
		/// <summary>
		/// Creates an int input
		/// </summary>
		public static int Int(string header, int value) => InternalCalls.gui_Int(header, value);
		/// <summary>
		/// Creates an Int Slider, clamped between min and max
		/// </summary>
		public static int SliderInt(string header, int value, int min, int max) => InternalCalls.gui_SliderInt(header, value, min, max);
		/// <summary>
		/// Creates an Float input
		/// </summary>
		public static float Float(string header, float value) => InternalCalls.gui_Float(header, value);
		/// <summary>
		/// Creates an Float Slider, clamped between min and max
		/// </summary>
		public static float SliderFloat(string header, float value, float min, float max) => InternalCalls.gui_SliderFloat(header, value, min, max);
		/// <summary>
		/// Creates an Vector3 input
		/// </summary>
		public static Vector3 Vector3(string header, Stulu.Vector3 value) { InternalCalls.gui_Vector3(header, ref value);return value; }
		/// <summary>
		/// Creates an Vector3 input, value is passed by refrence and can be changed by the engine
		/// </summary>
		public static bool Vector3(string header, ref Stulu.Vector3 value) => InternalCalls.gui_Vector3(header, ref value) == 1 ? true : false;

	}
}
