
using Stulu;
using System;
using System.Reflection;

namespace Editor
{
	/// <summary>
	/// API for interacting with 
	/// </summary>
	public static class Editor {
		/// <summary>
		/// Returns true if the editor is currently in rumtime mode (The play button is pressed)
		/// </summary>
		public static bool IsRuntime() => EditorCalls.Editor_IsRuntime();

		static Vector3 value = Vector3.Zero;
		static void DebugTest()
		{
			if (!IsRuntime())
			{
				if (ImGui.Vector3("Test Value", ref value, 0.5f))
					Log.Info("Value Channged");

			}
		}
		
	}
}
