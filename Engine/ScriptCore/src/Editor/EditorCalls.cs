using Stulu;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Editor
{
	static internal class EditorCalls {
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool Editor_IsRuntime();

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool ImGui_Vector3(string name, ref Vector3 value, float speed = 1.0f);
	}
}
