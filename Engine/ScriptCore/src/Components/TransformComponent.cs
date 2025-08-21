using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stulu {
	/// <summary>
	/// Holds information about the transform of a gameobject
	/// </summary>
	public class TransformComponent : GameObjectAttached {
		/// <summary>
		/// World position of the gameobject
		/// </summary>
		public Vector3 position { get { InternalCalls.transformComp_getPos(id, out Vector3 val); return val; } set => InternalCalls.transformComp_setPos(id, ref value); }
		/// <summary>
		/// Local position of the gameobject to the parent
		/// </summary>
		public Vector3 worldPosition { get { InternalCalls.transformComp_getWorldPos(id, out Vector3 val); return val; } }
		/// <summary>
		/// World rotation of the gameobject
		/// </summary>
		public Quaternion rotation { get { InternalCalls.transformComp_getRotation(id, out Quaternion val); return val; } }
		/// <summary>
		/// Local rotation of the gameobject to the parent
		/// </summary>
		public Quaternion worldRotation { get { InternalCalls.transformComp_getWorldRotation(id, out Quaternion val); return val; } }
		/// <summary>
		/// Local rotation of the gameobjectin in euler angles
		/// </summary>
		public Vector3 eulerAngles => rotation.ToEuler();
		/// <summary>
		/// World scale of the gameobject
		/// </summary>
		public Vector3 scale { get { InternalCalls.transformComp_getScale(id, out Vector3 val); return val; } set => InternalCalls.transformComp_setScale(id, ref value); }
		/// <summary>
		/// Local scale of the gameobject to the parent
		/// </summary>
		public Vector3 worldScale { get { InternalCalls.transformComp_getWorldScale(id, out Vector3 val); return val; } }

		/// <summary>
		/// Up vector for the gameobject
		/// </summary>
		public Vector3 up { get { InternalCalls.transformComp_up(id, out Vector3 val); return val; } }
		/// <summary>
		/// Right vector for the gameobject
		/// </summary>
		public Vector3 right { get { InternalCalls.transformComp_right(id, out Vector3 val); return val; } }
		/// <summary>
		/// Forward vector for the gameobject
		/// </summary>
		public Vector3 forward { get { InternalCalls.transformComp_forward(id, out Vector3 val); return val; } }

		/// <summary>
		/// Sets the objects rotation in radians
		/// </summary>
		/// <param name="rotation"></param>
		public void setRotation(Quaternion rotation) => InternalCalls.transformComp_setRotation(id, ref rotation);
	}
}
