using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stulu {
	public class TransformComponent : GameObjectAttached {
		public Vector3 position { get { Vector3 val; InternalCalls.transformComp_getPos(id, out val); return val; } set => InternalCalls.transformComp_setPos(id, ref value); }
		public Quaternion rotation { get { Quaternion val; InternalCalls.transformComp_getRotation(id, out val); return val; } }
		public Vector3 eulerAngles { get { Vector3 val; InternalCalls.transformComp_getRotation(id, out val); return val; } }
		public Vector3 scale { get { Vector3 val; InternalCalls.transformComp_getScale(id, out val); return val; } set => InternalCalls.transformComp_setScale(id, ref value); }

		public Vector3 up { get { Vector3 val; InternalCalls.transformComp_up(id, out val); return val; } }
		public Vector3 right { get { Vector3 val; InternalCalls.transformComp_right(id, out val); return val; } }
		public Vector3 forward { get { Vector3 val; InternalCalls.transformComp_forward(id, out val); return val; } }

		public void setRotation(Quaternion rotation) => InternalCalls.transformComp_setRotation(id, ref rotation);
	}
}
