using System;
using System.Runtime.CompilerServices;

namespace Stulu {
	public static class InternalCalls {
		//Stulu/Core/Application.h
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void application_exit(int code);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static uint application_getWidth();
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static uint application_getHeight();

		//Stulu/Core/Log.h
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void log_engine_log(int level, string message);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void log_client_log(int level, string message);

		//Stulu/Core/Time.h
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static float time_getFrameTime();
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static float time_getDeltaTime();
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static float time_getApplicationRuntime();
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static float time_getScale();
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static float time_getTime();
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void time_setScale(float scale);


		//Stulu/Core/Input.h
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool input_keyDown(uint keycode);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool input_mouseDown(uint mouseCode);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void input_setCursorMode(uint mode);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static uint input_getCursorMode();
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void input_getMousePos(out Vector2 pos);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void input_getMouseDelta(out Vector2 delta);

		//Stulu/Scene/GameObject.h
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static uint gameObject_create(String name, String tag, Vector3 postion, Quaternion rotation, Vector3 Scale);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void gameObject_add_component(uint entityId, Type t);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool gameObject_has_component(uint entityId, Type t);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool gameObject_remove_component(uint entityId, Type t);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static String gameObject_getName(uint entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void gameObject_setName(uint entityId, String name);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static String gameObject_getTag(uint entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void gameObject_setTag(uint entityId, String tag);


		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void transformComp_setPos(uint entityId, ref Vector3 pos);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void transformComp_getPos(uint entityId, out Vector3 pos);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void transformComp_getWorldPos(uint entityId, out Vector3 pos);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void transformComp_setRotation(uint entityId, ref Quaternion rotation);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void transformComp_getRotation(uint entityId, out Quaternion rotation);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void transformComp_getRotation(uint entityId, out Vector3 degrees);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void transformComp_getWorldRotation(uint entityId, out Quaternion rotation);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void transformComp_setScale(uint entityId, ref Vector3 scale);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void transformComp_getScale(uint entityId, out Vector3 scale);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void transformComp_getWorldScale(uint entityId, out Vector3 scale);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void transformComp_right(uint entityId, out Vector3 right);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void transformComp_forward(uint entityId, out Vector3 fronz);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void transformComp_up(uint entityId, out Vector3 up);
	}
}
