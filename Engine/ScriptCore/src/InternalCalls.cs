using System;
using System.Data;
using System.Runtime.CompilerServices;

namespace Stulu {
	using IDType = System.UInt64;
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
		internal extern static ulong gameObject_create(String name, String tag, Vector3 postion, Quaternion rotation, Vector3 Scale);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static ulong gameObject_destroy(ulong entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void gameObject_add_component(ulong entityId, Type t);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool gameObject_has_component(ulong entityId, Type t);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool gameObject_remove_component(ulong entityId, Type t);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static Object gameObject_get_component(ulong entityId, Type t);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static String gameObject_getName(ulong entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void gameObject_setName(ulong entityId, String name);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static String gameObject_getTag(ulong entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void gameObject_setTag(ulong entityId, String tag);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static ulong gameObject_createSphere(String name, String tag, Vector3 postion);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static ulong gameObject_createCube(String name, String tag, Vector3 postion);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static ulong gameObject_createCapsule(String name, String tag, Vector3 postion);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static ulong gameObject_createPlane(String name, String tag, Vector3 postion);
		#endregion
		#region Transform
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void transformComp_setPos(ulong entityId, ref Vector3 pos);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void transformComp_getPos(ulong entityId, out Vector3 pos);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void transformComp_getWorldPos(ulong entityId, out Vector3 pos);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void transformComp_setRotation(ulong entityId, ref Quaternion rotation);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void transformComp_getRotation(ulong entityId, out Quaternion rotation);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void transformComp_getWorldRotation(ulong entityId, out Quaternion rotation);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void transformComp_setScale(ulong entityId, ref Vector3 scale);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void transformComp_getScale(ulong entityId, out Vector3 scale);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void transformComp_getWorldScale(ulong entityId, out Vector3 scale);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void transformComp_right(ulong entityId, out Vector3 right);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void transformComp_forward(ulong entityId, out Vector3 fronz);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void transformComp_up(ulong entityId, out Vector3 up);
		#endregion
		#region Rigidbody
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void rigidbody_addForce(ulong entityId, ref Vector3 force, uint forcemode);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool rigidbody_useGravity(ulong entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool rigidbody_useGravity(ulong entityId, bool value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool rigidbody_kinematic(ulong entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool rigidbody_kinematic(ulong entityId, bool value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool rigidbody_retainaccel(ulong entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool rigidbody_retainaccel(ulong entityId, bool value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static float rigidbody_mass(ulong entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static float rigidbody_mass(ulong entityId, float value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void rigidbody_massCenterGet(ulong entityId, out Vector3 scale);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void rigidbody_massCenterSet(ulong entityId, ref Vector3 value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool rigidbody_lockAngularVelocity(ulong entityId, int index);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void rigidbody_lockAngularVelocity(ulong entityId, int index, bool value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool rigidbody_lockLinearVelocity(ulong entityId, int index);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void rigidbody_lockLinearVelocity(ulong entityId, int index, bool value);
		#endregion
		#region BoxCollider
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void boxcollider_getMaterial(ulong entityId, out PhysicsMaterial materialRef);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void boxcollider_updateMaterial(ulong entityId, ref PhysicsMaterial materialRef);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void boxcollider_getOffset(ulong entityId, out Vector3 value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void boxcollider_setOffset(ulong entityId, ref Vector3 value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void boxcollider_getSize(ulong entityId, out Vector3 value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void boxcollider_setSize(ulong entityId, ref Vector3 value);
		#endregion
		#region SphereCollider
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void spherecollider_getMaterial(ulong entityId, out PhysicsMaterial materialRef);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void spherecollider_updateMaterial(ulong entityId, ref PhysicsMaterial materialRef);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void spherecollider_getOffset(ulong entityId, out Vector3 value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void spherecollider_setOffset(ulong entityId, ref Vector3 value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static float spherecollider_radius(ulong entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void spherecollider_radius(ulong entityId, float value);
		#endregion
		#region CapsuleCollider
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void capsuleCollider_getMaterial(ulong entityId, out PhysicsMaterial materialRef);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void capsuleCollider_updateMaterial(ulong entityId, ref PhysicsMaterial materialRef);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void capsuleCollider_getOffset(ulong entityId, out Vector3 value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void capsuleCollider_setOffset(ulong entityId, ref Vector3 value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static float capsuleCollider_radius(ulong entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void capsuleCollider_radius(ulong entityId, float value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static float capsuleCollider_height(ulong entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void capsuleCollider_height(ulong entityId, float value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool capsuleCollider_vertical(ulong entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void capsuleCollider_vertical(ulong entityId, bool value);
		#endregion
		#region Renderer
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void renderer2D_drawLine(ref Vector3 start, ref Vector3 end, ref Vector4 color);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void renderer2D_drawQuad(ref Vector3 pos, ref Quaternion rotation, ref Vector3 scale, ref Vector4 color);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void renderer2D_drawTexturedQuad(ref Vector3 pos, ref Quaternion rotation, ref Vector3 scale, ref Vector4 color, ref Vector2 tiling, IDType textureID);
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
		internal extern static void gizmo_drawRect(ref Matrix4 matrix, ref Vector4 color);
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
		internal extern static float texture2d_getWidth(IDType textureID);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static float texture2d_getHeight(IDType textureID);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static string texture2d_getPath(IDType textureID);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static IDType texture2d_findbypath(string path);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static IDType texture2d_getWhiteTexture();
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static IDType texture2d_getBlackTexture();
		#endregion
		#region Folders
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static string folders_assetPath();
		#endregion
		#region SpriteRenderer
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static IDType spriteRenComp_getTexture(ulong entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void spriteRenComp_setTexture(ulong entityId, IDType id);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void spriteRenComp_getTiling(ulong entityId, out Vector2 tiling);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void spriteRenComp_setTiling(ulong entityId, ref Vector2 tiling);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void spriteRenComp_getColor(ulong entityId, out Vector4 tiling);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void spriteRenComp_setColor(ulong entityId, ref Vector4 tiling);
		#endregion
		#region ScriptingComponent
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static Component[] scriptingComp_fetchScripts(ulong entityId);
		#endregion
		#region CameraComponent
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static float cameraComp_fov(ulong entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void cameraComp_fov(ulong entityId, float value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static float cameraComp_far(ulong entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void cameraComp_far(ulong entityId, float value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static int cameraComp_clearType(ulong entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void cameraComp_clearType(ulong entityId, int value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static float cameraComp_near(ulong entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void cameraComp_near(ulong entityId, float value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void cameraComp_update(ulong entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static float cameraComp_aspect(ulong entityId);
		#endregion
		#region Quaternion
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static float quaternion_fromEuler(out Quaternion outQuat, ref Vector3 euler);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static float quaternion_toEuler(ref Quaternion quat, out Vector3 outEuler);

		#endregion
	}
}
