using Stulu;
using System.Runtime.InteropServices;

namespace Editor {
	/// <summary>
	/// Gui interface for the Editor GUI
	/// </summary>
	public static class ImGui 
	{
		/// <summary>
		/// Draws a vector 3 control
		/// </summary>
		/// <returns>If the value has changed</returns>
		public static bool Vector3(string name, ref Vector3 value, float speed = 1.0f) => EditorCalls.ImGui_Vector3(name, ref value, speed);
		/// <summary>
		/// Draws a float control
		/// </summary>
		/// <returns>If the value has changed</returns>
		public static bool Float(string name, ref float value, float min = System.Single.MinValue, float max = System.Single.MaxValue, float speed = 1.0f) => EditorCalls.ImGui_Float(name, ref value, min, max, speed);
		/// <summary>
		/// Draws a checkbox
		/// </summary>
		/// <returns>If the value has changed</returns>
		public static bool Bool(string name, ref bool value) => EditorCalls.ImGui_Bool(name, ref value);
	}
}
