using System;
using System.Runtime.CompilerServices;


namespace Stulu.Editor {
	public static class InternalCalls {
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static int gui_Begin(string header);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void gui_End();
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void gui_Text(string text);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern int gui_CheckBox(string header, int value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern int gui_SliderInt(string header, int value, int min, int max);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern int gui_Int(string header, int value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float gui_SliderFloat(string header, float value, float max, float min);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float gui_Float(string header, float value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern int gui_Vector3(string header, ref Stulu.Vector3 inValue);
	}
}