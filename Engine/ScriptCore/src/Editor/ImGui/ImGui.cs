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
	}
}
