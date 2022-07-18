using System;
using System.Runtime.CompilerServices;

namespace Stulu {
	static internal class InternalCalls {
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
		internal extern static bool input_getKeyDown(uint keycode);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool input_getKeyUp(uint keycode);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool input_getKey(uint keycode);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool input_getMouseDown(uint mouseCode);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool input_getMouseUp(uint mouseCode);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool input_getMouse(uint mouseCode);
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
		internal extern static void transformComp_getRotation(uint entityId, out Vector3 eulerAngles);
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

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void rigidbody_addForce(uint entityId, ref Vector3 force, uint forcemode);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool rigidbody_useGravity(uint entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool rigidbody_useGravity(uint entityId, bool value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool rigidbody_kinematic(uint entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool rigidbody_kinematic(uint entityId, bool value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool rigidbody_retainaccel(uint entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool rigidbody_retainaccel(uint entityId, bool value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static float rigidbody_mass(uint entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static float rigidbody_mass(uint entityId, float value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void rigidbody_massCenterGet(uint entityId, out Vector3 scale);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void rigidbody_massCenterSet(uint entityId, ref Vector3 value);

		//Renderer
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void renderer2D_drawLine(ref Vector3 start, ref Vector3 end, ref Vector4 color);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void renderer2D_drawQuad(ref Vector3 pos, ref Quaternion rotation, ref Vector3 scale, ref Vector4 color);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void renderer2D_drawCircle(ref Vector3 pos, ref Quaternion rotation, ref Vector3 scale, ref Vector4 color, float thickness, float fade);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void renderer2D_drawLineRect(ref Vector3 pos, ref Quaternion rotation, ref Vector3 scale, ref Vector4 color);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void renderer2D_drawLineCube(ref Vector3 pos, ref Quaternion rotation, ref Vector3 scale, ref Vector4 color);
	}
}
