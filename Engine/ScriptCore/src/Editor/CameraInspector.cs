using Editor;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stulu.src.Editor
{
	[InspectorRenderer(typeof(CameraComponent), "Camera")]
	internal class CameraInspector : Inspector
	{
		private static readonly Vector4[] edgesPos = new Vector4[4] {
				new Vector4( -0.5f, -0.5f, 0.0f, 1.0f ),
				new Vector4( 0.5f, -0.5f, 0.0f, 1.0f ),
				new Vector4( 0.5f,  0.5f, 0.0f, 1.0f ),
				new Vector4( -0.5f,  0.5f, 0.0f, 1.0f ),
		};


		public override void Render(GameObject gameObject)
		{
			CameraComponent camera = gameObject.getComponent<CameraComponent>();
			if (camera == null)  return;

			float far = camera.Far;
			float near = camera.Near;
			float fov = camera.Fov;
			int clearType = (int)camera.ClearType;

			if(ImGui.Float("Far", ref far))
			{
				camera.Far = far;
				camera.Update();
			}

			if (ImGui.Float("Near", ref near))
			{
				camera.Near = near;
				camera.Update();
			}

			if (ImGui.Float("Fov", ref fov))
			{
				camera.Fov = fov;
				camera.Update();
			}

			if (ImGui.Combo("Clear Type", ref clearType, Enum.GetNames(typeof(ClearType))))
			{
				camera.ClearType = (ClearType)clearType;
			}
		}
		public override void RenderGizmos(GameObject gameObject)
		{
			/*
			CameraComponent camera = gameObject.getComponent<CameraComponent>();
			if (camera == null) return;

			TransformComponent transform = gameObject.transform;

			float near = camera.Near;
			float far = camera.Far;
			float halfFOV = camera.Fov * 0.5f;
			float aspect = camera.AspectRatio;

			float nearHeight = 1.0f;
			float nearWidth = nearHeight * aspect;
			float farHeight = 2.0f * far * Mathf.Tan(halfFOV); // pythagoras
			float farWidth = farHeight * aspect;
			Vector3 nearPos = transform.worldPosition + transform.forward * near;
			Vector3 farPos = transform.worldPosition + transform.forward * far;

			Matrix4 nearTransform = Matrix4.Translate(nearPos) * Matrix4.Rotate(transform.worldRotation) * Matrix4.Scale(new Vector3(nearWidth, nearHeight, 1.0f));
			Matrix4 farTransform = Matrix4.Translate(farPos) * Matrix4.Rotate(transform.worldRotation) * Matrix4.Scale(new Vector3(farWidth, farHeight, 1.0f));

			Gizmo.drawOutlinedQuad(ref nearTransform, Color.White);
			Gizmo.drawOutlinedQuad(ref farTransform, Color.White);

			
			for (int i = 0; i < 4; i++) {
				Gizmo.drawLine((nearTransform * edgesPos[i]).xyz, (farTransform * edgesPos[i]).xyz, Color.White);
			}
			*/
			 
		}
	}
}
