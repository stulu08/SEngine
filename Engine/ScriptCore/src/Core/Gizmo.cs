using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stulu {
	/// <summary>
	/// Direct Acces to the Engine Editor Gizmo Renderer, can not be seen in the Game Window, only in Editor Window
	/// </summary>
	public static class Gizmo {
		/// <summary>
		/// Draws a Ray
		/// </summary>
		public static void drawRay(Vector3 start, Vector3 direction, float length, Vector4 Color) {
			Vector3 end = (start + direction * length);
			InternalCalls.gizmo_drawLine(ref start, ref end, ref Color);
		}
		/// <summary>
		/// Draws a Line
		/// </summary>
		public static void drawLine(Vector3 start, Vector3 end, Vector4 Color) => InternalCalls.gizmo_drawLine(ref start, ref end, ref Color);
		/// <summary>
		/// Draws a Line
		/// </summary>
		public static void drawLine(Vector2 start, Vector2 end, Vector4 Color) {
			Vector3 vecStart = new Vector3(start, 0);
			Vector3 vecEnd = new Vector3(end, 0);
			InternalCalls.gizmo_drawLine(ref vecStart, ref vecEnd, ref Color);
		}
		#region Cubes
		/// <summary>
		/// Draws a cube out of 10 lines
		/// </summary>
		public static void drawOutlinedCube(Vector3 position, Vector4 Color) {
			Vector3 scale = new Vector3(1);
			Quaternion rotation = new Quaternion(0, 0, 0, 1);
			InternalCalls.gizmo_drawOutlineCube(ref position, ref rotation, ref scale, ref Color);
		}
		/// <summary>
		/// Draws a cube out of 10 lines
		/// </summary>
		public static void drawOutlinedCube(Vector3 position, Vector3 scale, Vector4 Color) {
			Quaternion rotation = new Quaternion(0, 0, 0, 1);
			InternalCalls.gizmo_drawOutlineCube(ref position, ref rotation, ref scale, ref Color);
		}
		/// <summary>
		/// Draws a cube out of 10 lines
		/// </summary>
		public static void drawOutlinedCube(Vector3 position, Vector3 scale, Quaternion rotation, Vector4 Color) => InternalCalls.gizmo_drawOutlineCube(ref position, ref rotation, ref scale, ref Color);
		#endregion
		#region QUAD
		/// <summary>
		/// Draws a 2D Quad
		/// </summary>
		public static void drawQuad(Vector3 position, Vector4 color) {
			Quaternion rotation = new Quaternion(0, 0, 0, 1);
			Vector3 scale = new Vector3(1);
			InternalCalls.renderer2D_drawQuad(ref position, ref rotation, ref scale, ref color);
		}
		/// <summary>
		/// Draws a 2D Quad
		/// </summary>
		public static void drawQuad(Vector3 position, Vector3 scale, Vector4 color) {
			Quaternion rotation = new Quaternion(0, 0, 0, 1);
			InternalCalls.renderer2D_drawQuad(ref position, ref rotation, ref scale, ref color);
		}
		/// <summary>
		/// Draws a 2D Quad
		/// </summary>
		public static void drawQuad(Vector3 position, Vector3 scale, Quaternion rotation, Vector4 color) => InternalCalls.renderer2D_drawQuad(ref position, ref rotation, ref scale, ref color);
		/// <summary>
		/// Draws a 2D Quad
		/// </summary>
		public static void drawQuad(Vector2 position, Vector4 color) {
			Vector3 _position = new Vector3(position, 0);
			Quaternion _rotation = new Quaternion(0, 0, 0, 1);
			Vector3 _scale = new Vector3(1);
			InternalCalls.renderer2D_drawQuad(ref _position, ref _rotation, ref _scale, ref color);
		}
		/// <summary>
		/// Draws a 2D Quad
		/// </summary>
		public static void drawQuad(Vector2 position, Vector2 scale, Vector4 color) {
			Vector3 _position = new Vector3(position, 0);
			Quaternion _rotation = new Quaternion(0, 0, 0, 1);
			Vector3 _scale = new Vector3(scale, 1);
			InternalCalls.renderer2D_drawQuad(ref _position, ref _rotation, ref _scale, ref color);
		}
		/// <summary>
		/// Draws a 2D Quad
		/// </summary>
		public static void drawQuad(Vector2 position, Vector2 scale, float rotation, Vector4 color) {
			Vector3 _position = new Vector3(position, 0);
			Quaternion _rotation = Quaternion.EulerDegree(new Vector3(0, 0, rotation));
			Vector3 _scale = new Vector3(scale, 1);
			InternalCalls.renderer2D_drawQuad(ref _position, ref _rotation, ref _scale, ref color);
		}
		#endregion
		#region OutlinedQuad
		/// <summary>
		/// Draws a 2D Outlined Quad
		/// </summary>
		public static void drawOutlinedQuad(Vector3 position, Vector4 color) {
			Quaternion rotation = new Quaternion(0, 0, 0, 1);
			Vector3 scale = new Vector3(1);
			InternalCalls.gizmo_drawRect(ref position, ref rotation, ref scale, ref color);
		}
		/// <summary>
		/// Draws a 2D Outlined Quad
		/// </summary>
		public static void drawOutlinedQuad(Vector3 position, Vector3 scale, Vector4 color) {
			Quaternion rotation = new Quaternion(0, 0, 0, 1);
			InternalCalls.gizmo_drawRect(ref position, ref rotation, ref scale, ref color);
		}
		/// <summary>
		/// Draws a 2D Outlined Quad
		/// </summary>
		public static void drawOutlinedQuad(Vector3 position, Vector3 scale, Quaternion rotation, Vector4 color) => InternalCalls.gizmo_drawRect(ref position, ref rotation, ref scale, ref color);
		/// <summary>
		/// Draws a 2D Outlined Quad
		/// </summary>
		public static void drawOutlinedQuad(Vector2 position, Vector4 color) {
			Vector3 _position = new Vector3(position, 0);
			Quaternion _rotation = new Quaternion(0, 0, 0, 1);
			Vector3 _scale = new Vector3(1);
			InternalCalls.gizmo_drawRect(ref _position, ref _rotation, ref _scale, ref color);
		}
		/// <summary>
		/// Draws a 2D Outlined Quad
		/// </summary>
		public static void drawOutlinedQuad(Vector2 position, Vector2 scale, Vector4 color) {
			Vector3 _position = new Vector3(position, 0);
			Quaternion _rotation = new Quaternion(0, 0, 0, 1);
			Vector3 _scale = new Vector3(scale, 1);
			InternalCalls.gizmo_drawRect(ref _position, ref _rotation, ref _scale, ref color);
		}
		/// <summary>
		/// Draws a 2D Outlined Quad
		/// </summary>
		public static void drawOutlinedQuad(Vector2 position, Vector2 scale, float rotation, Vector4 color) {
			Vector3 _position = new Vector3(position, 0);
			Quaternion _rotation = Quaternion.EulerDegree(new Vector3(0, 0, rotation));
			Vector3 _scale = new Vector3(scale, 1);
			InternalCalls.gizmo_drawRect(ref _position, ref _rotation, ref _scale, ref color);
		}
		/// <summary>
		/// Draws a 2D Outlined Quad
		/// </summary>
		public static void drawOutlinedQuad(ref Matrix4 matrix, Vector4 color)
		{
			InternalCalls.gizmo_drawRect(ref matrix, ref color);
		}
		#endregion
		#region Circle
		/// <summary>
		/// Draws a 2D Circle
		/// </summary>
		public static void drawCircle(Vector3 position, Vector4 color, float thickness = 1.0f, float fade = 0.005f) {
			Quaternion rotation = new Quaternion(0, 0, 0, 1);
			Vector3 scale = new Vector3(1);
			InternalCalls.gizmo_drawCircle(ref position, ref rotation, ref scale, ref color, thickness, fade);
		}
		/// <summary>
		/// Draws a 2D Circle
		/// </summary>
		public static void drawCircle(Vector3 position, Vector3 scale, Vector4 color, float thickness = 1.0f, float fade = 0.005f) {
			Quaternion rotation = new Quaternion(0, 0, 0, 1);
			InternalCalls.gizmo_drawCircle(ref position, ref rotation, ref scale, ref color, thickness, fade);
		}
		/// <summary>
		/// Draws a 2D Circle
		/// </summary>
		public static void drawCircle(Vector3 position, Vector3 scale, Quaternion rotation, Vector4 color, float thickness = 1.0f, float fade = 0.005f) => InternalCalls.gizmo_drawCircle(ref position, ref rotation, ref scale, ref color, thickness, fade);
		/// <summary>
		/// Draws a 2D Circle
		/// </summary>
		public static void drawCircle(Vector2 position, Vector4 color, float thickness = 1.0f, float fade = 0.005f) {
			Vector3 _position = new Vector3(position, 0);
			Quaternion _rotation = new Quaternion(0, 0, 0, 1);
			Vector3 _scale = new Vector3(1);
			InternalCalls.gizmo_drawCircle(ref _position, ref _rotation, ref _scale, ref color, thickness, fade);
		}
		/// <summary>
		/// Draws a 2D Circle
		/// </summary>
		public static void drawCircle(Vector2 position, Vector2 scale, Vector4 color, float thickness = 1.0f, float fade = 0.005f) {
			Vector3 _position = new Vector3(position, 0);
			Quaternion _rotation = new Quaternion(0, 0, 0, 1);
			Vector3 _scale = new Vector3(scale, 1);
			InternalCalls.gizmo_drawCircle(ref _position, ref _rotation, ref _scale, ref color, thickness, fade);
		}
		/// <summary>
		/// Draws a 2D Circle
		/// </summary>
		public static void drawCircle(Vector2 position, Vector2 scale, float rotation, Vector4 color, float thickness = 1.0f, float fade = 0.005f) {
			Vector3 _position = new Vector3(position, 0);
			Quaternion _rotation = Quaternion.EulerDegree(new Vector3(0, 0, rotation));
			Vector3 _scale = new Vector3(scale, 1);
			InternalCalls.gizmo_drawCircle(ref _position, ref _rotation, ref _scale, ref color, thickness, fade);
		}
		#endregion
		#region Cubes
		/// <summary>
		/// Draws a cube
		/// </summary>
		public static void drawCube(Vector3 position, Vector4 Color) {
			Vector3 scale = new Vector3(1);
			Quaternion rotation = new Quaternion(0, 0, 0, 1);
			InternalCalls.gizmo_drawCube(ref position, ref rotation, ref scale, ref Color);
		}
		/// <summary>
		/// Draws a cube
		/// </summary>
		public static void drawCube(Vector3 position, Vector3 scale, Vector4 Color) {
			Quaternion rotation = new Quaternion(0, 0, 0, 1);
			InternalCalls.gizmo_drawCube(ref position, ref rotation, ref scale, ref Color);
		}
		/// <summary>
		/// Draws a cube
		/// </summary>
		public static void drawCube(Vector3 pos, Vector3 scale, Quaternion rotation, Vector4 Color) => InternalCalls.gizmo_drawCube(ref pos, ref rotation, ref scale, ref Color);

		#endregion
		#region Spheres
		/// <summary>
		/// Draws a sphere
		/// </summary>
		public static void drawSphere(Vector3 position, Vector4 Color) {
			Vector3 scale = new Vector3(1);
			Quaternion rotation = new Quaternion(0, 0, 0, 1);
			InternalCalls.gizmo_drawSphere(ref position, ref rotation, ref scale, ref Color);
		}
		/// <summary>
		/// Draws a sphere
		/// </summary>
		public static void drawSphere(Vector3 position, Vector3 scale, Vector4 Color) {
			Quaternion rotation = new Quaternion(0, 0, 0, 1);
			InternalCalls.gizmo_drawSphere(ref position, ref rotation, ref scale, ref Color);
		}
		/// <summary>
		/// Draws a sphere
		/// </summary>
		public static void drawSphere(Vector3 pos, Vector3 scale, Quaternion rotation, Vector4 Color) => InternalCalls.gizmo_drawSphere(ref pos, ref rotation, ref scale, ref Color);

		#endregion
	}
}
