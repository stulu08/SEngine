using System.Runtime.CompilerServices;

namespace Stulu.Editor {
	public class Gui {
		public static bool Begin(string header) => (InternalCalls.gui_Begin(header) == 1 ? true : false);
		public static void End() => InternalCalls.gui_End();
		public static void Text(string header) => InternalCalls.gui_Text(header);
		public static bool CheckBox(string header, bool value) => InternalCalls.gui_CheckBox(header, value ? 1 : 0) == 0 ? false : true;
		public static int Int(string header, int value) => InternalCalls.gui_Int(header, value);
		public static int SliderInt(string header, int value, int min, int max) => InternalCalls.gui_SliderInt(header, value, min, max);
		public static float Float(string header, float value) => InternalCalls.gui_Float(header, value);
		public static float SliderFloat(string header, float value, float min, float max) => InternalCalls.gui_SliderFloat(header, value, min, max);
		public static Vector3 Vector3(string header, Stulu.Vector3 value) { InternalCalls.gui_Vector3(header, ref value);return value; }
		public static bool Vector3(string header, ref Stulu.Vector3 value) => InternalCalls.gui_Vector3(header, ref value) == 1 ? true : false;

	}
}
