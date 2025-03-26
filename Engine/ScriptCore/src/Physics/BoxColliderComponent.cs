#pragma warning disable 1591
namespace Stulu
{
	public class BoxColliderComponent : GameObjectAttached, Collider
	{
		public PhysicsMaterial Material
		{
			get
			{
				InternalCalls.boxcollider_getMaterial(id, out PhysicsMaterial mat);
				return mat;
			}
			set => InternalCalls.boxcollider_updateMaterial(id, ref value);
		}

		public Vector3 Offset
		{
			get
			{
				InternalCalls.boxcollider_getOffset(id, out Vector3 value);
				return value;
			}
			set => InternalCalls.boxcollider_setOffset(id, ref value);
		}

		public Vector3 Size 
		{ 
			get 
			{ 
				InternalCalls.boxcollider_getSize(id, out Vector3 value); 
				return value; 
			} set => InternalCalls.boxcollider_setSize(id, ref value); 
		}
	}
}
