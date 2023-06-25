using System;
using System.Runtime.CompilerServices;

namespace Stulu {
	using _UUID = System.UInt64;
	static internal class InternalCalls {
		#region Application
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void application_exit(int code);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static uint application_getWidth();
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static uint application_getHeight();
		#endregion
		#region Log
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void log_engine_log(int level, string message);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void log_client_log(int level, string message);
		#endregion
		#region Time
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
		#endregion
		#region Input
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
		#endregion
		#region GameObject
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static uint gameObject_create(String name, String tag, Vector3 postion, Quaternion rotation, Vector3 Scale);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static uint gameObject_destroy(uint entityId);
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
		internal extern static uint gameObject_createSphere(String name, String tag, Vector3 postion);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static uint gameObject_createCube(String name, String tag, Vector3 postion);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static uint gameObject_createCapsule(String name, String tag, Vector3 postion);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static uint gameObject_createPlane(String name, String tag, Vector3 postion);
		#endregion
		#region Transform
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
		#endregion
		#region Rigidbody
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
		#endregion
		#region Renderer
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void renderer2D_drawLine(ref Vector3 start, ref Vector3 end, ref Vector4 color);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void renderer2D_drawQuad(ref Vector3 pos, ref Quaternion rotation, ref Vector3 scale, ref Vector4 color);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void renderer2D_drawTexturedQuad(ref Vector3 pos, ref Quaternion rotation, ref Vector3 scale, ref Vector4 color, ref Vector2 tiling, _UUID textureID);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void renderer2D_drawCircle(ref Vector3 pos, ref Quaternion rotation, ref Vector3 scale, ref Vector4 color, float thickness, float fade);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void renderer2D_drawLineRect(ref Vector3 pos, ref Quaternion rotation, ref Vector3 scale, ref Vector4 color);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void renderer2D_drawLineCube(ref Vector3 pos, ref Quaternion rotation, ref Vector3 scale, ref Vector4 color);
		#endregion
		#region Gizmo
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void gizmo_drawLine(ref Vector3 begin, ref Vector3 end, ref Vector4 color);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void gizmo_drawRect(ref Vector3 position, ref Quaternion rotation, ref Vector3 scale, ref Vector4 color);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void gizmo_drawOutlineCube(ref Vector3 position, ref Quaternion rotation, ref Vector3 scale, ref Vector4 color);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void gizmo_drawCircle(ref Vector3 position, ref Quaternion rotation, ref Vector3 scale, ref Vector4 color, float thickness, float fade);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void gizmo_drawCube(ref Vector3 position, ref Quaternion rotation, ref Vector3 scale, ref Vector4 color);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void gizmo_drawSphere(ref Vector3 position, ref Quaternion rotation, ref Vector3 scale, ref Vector4 color);
		#endregion
		#region Texture2D
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static float texture2d_getWidth(_UUID textureID);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static float texture2d_getHeight(_UUID textureID);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static string texture2d_getPath(_UUID textureID);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static _UUID texture2d_findbypath(string path);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static _UUID texture2d_getWhiteTexture();
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static _UUID texture2d_getBlackTexture();
		#endregion
		#region Folders
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static string folders_assetPath();
		#endregion
		#region SpriteRenderer
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static _UUID spriteRenComp_getTexture(uint entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void spriteRenComp_setTexture(uint entityId, _UUID id);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void spriteRenComp_getTiling(uint entityId, out Vector2 tiling);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void spriteRenComp_setTiling(uint entityId, ref Vector2 tiling);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void spriteRenComp_getColor(uint entityId, out Vector4 tiling);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void spriteRenComp_setColor(uint entityId, ref Vector4 tiling);
		#endregion
	}
}
