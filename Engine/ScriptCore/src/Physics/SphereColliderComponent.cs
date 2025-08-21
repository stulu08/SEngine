#pragma warning disable 1591
namespace Stulu
{
	public class SphereColliderComponent : GameObjectAttached, Collider
	{
		public PhysicsMaterial Material
		{
			get
			{
				InternalCalls.spherecollider_getMaterial(id, out PhysicsMaterial mat);
				return mat;
			}
			set => InternalCalls.spherecollider_updateMaterial(id, ref value);
		}

		public Vector3 Offset
		{
			get
			{
				InternalCalls.spherecollider_getOffset(id, out Vector3 value);
				return value;
			}
			set => InternalCalls.spherecollider_setOffset(id, ref value);
		}

		public float Radius
		{
			get => InternalCalls.spherecollider_radius(id);
			set => InternalCalls.spherecollider_radius(id, value);
		}
	}
}
