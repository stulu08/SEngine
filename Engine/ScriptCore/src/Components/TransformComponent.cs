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
		/// World position of the gameobject
		public Vector3 position { get { Vector3 val; InternalCalls.transformComp_getPos(id, out val); return val; } set => InternalCalls.transformComp_setPos(id, ref value); }
		/// Local position of the gameobject to the parent
		public Vector3 worldPosition { get { Vector3 val; InternalCalls.transformComp_getWorldPos(id, out val); return val; } }
		/// World rotation of the gameobject
		public Quaternion rotation { get { Quaternion val; InternalCalls.transformComp_getRotation(id, out val); return val; } }
		/// Local rotation of the gameobject to the parent
		public Quaternion worldRotation { get { Quaternion val; InternalCalls.transformComp_getWorldRotation(id, out val); return val; } }
		/// World rotation of the gameobjectin euler angles
		public Vector3 eulerAngles { get { Vector3 val; InternalCalls.transformComp_getRotation(id, out val); return val; } }
		/// World scale of the gameobject
		public Vector3 scale { get { Vector3 val; InternalCalls.transformComp_getScale(id, out val); return val; } set => InternalCalls.transformComp_setScale(id, ref value); }
		/// Local scale of the gameobject to the parent
		public Vector3 worldScale { get { Vector3 val; InternalCalls.transformComp_getWorldScale(id, out val); return val; } }

		/// Up vector for the gameobject
		public Vector3 up { get { Vector3 val; InternalCalls.transformComp_up(id, out val); return val; } }
		/// Right vector for the gameobject
		public Vector3 right { get { Vector3 val; InternalCalls.transformComp_right(id, out val); return val; } }
		/// Forward vector for the gameobject
		public Vector3 forward { get { Vector3 val; InternalCalls.transformComp_forward(id, out val); return val; } }

		///  Sets the objects rotation
		public void setRotation(Quaternion rotation) => InternalCalls.transformComp_setRotation(id, ref rotation);
	}
}
