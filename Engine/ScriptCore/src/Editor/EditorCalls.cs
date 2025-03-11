using Stulu;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Editor
{
	static internal class EditorCalls {
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool Editor_IsRuntime();

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool ImGui_Vector3(string name, ref Vector3 value, float speed);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool ImGui_Float(string name, ref float value, float min, float max, float speed);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool ImGui_Bool(string name, ref bool value);
	}
}
