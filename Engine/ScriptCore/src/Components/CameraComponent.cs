using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stulu
{
	/// <summary>
	/// Acces to camera settings
	/// </summary>
	public class CameraComponent : GameObjectAttached {
		/// <summary>
		/// Field of View
		/// </summary>
		public float Fov { get => InternalCalls.cameraComp_fov(id);  set => InternalCalls.cameraComp_fov(id, value); }
		/// <summary>
		/// The near value, how near can can something be renderer
		/// </summary>
		public float Near { get => InternalCalls.cameraComp_near(id); set => InternalCalls.cameraComp_near(id, value); }
		/// <summary>
		/// View distance
		/// </summary>
		public float Far { get => InternalCalls.cameraComp_far(id); set => InternalCalls.cameraComp_far(id, value); }
		/// <summary>
		/// Aspect ratio of the camera ( width/height )
		/// </summary>
		public float AspectRatio { get => InternalCalls.cameraComp_aspect(id); }

		/// <summary>
		/// Applies the changed to the camera
		/// </summary>
		public void Update() => InternalCalls.cameraComp_update(id);
	}
}
