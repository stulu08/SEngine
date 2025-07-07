using Stulu;
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Editor
{
	using IDType = System.UInt64;
	static internal class EditorCalls
	{
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool Editor_IsRuntime();
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool Editor_TypeHasInspector(Type type);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool ImGui_Text(string name);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool ImGui_Vector2(string name, ref Vector2 value, float speed);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool ImGui_Vector3(string name, ref Vector3 value, float speed);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool ImGui_Float(string name, ref float value, float min, float max, float speed);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool ImGui_Int(string name, ref int value, int min, int max, float speed);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool ImGui_UInt(string name, ref uint value, uint min, uint max, float speed);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool ImGui_Bool(string name, ref bool value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool ImGui_TreeNode(string name, uint flags);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void ImGui_TreePop();
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool ImGui_Combo(string name, ref int currentValue, string optionsString);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void ImGui_PushID(string id);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void ImGui_PopID();
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool ImGui_Button(string name);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool ImGui_InspectorTreeNodeIndent(string name, out bool outCloseBtn, string closeText);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void ImGui_InspectorTreePopUnindent();
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool ImGui_Texture2D(string name, ref IDType value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool ImGui_Material(string name, ref IDType value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool ImGui_Mesh(string name, ref IDType value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool ImGui_GameObject(string name, ref IDType value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool ImGui_SameLine(float offset, float spacing);
	}
}
