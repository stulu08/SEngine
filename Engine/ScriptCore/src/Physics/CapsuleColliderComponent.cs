#pragma warning disable 1591
namespace Stulu
{
	public class CapsuleColliderComponent : GameObjectAttached, Collider
	{
		public PhysicsMaterial Material
		{
			get
			{
				InternalCalls.capsuleCollider_getMaterial(id, out PhysicsMaterial mat);
				return mat;
			}
			set => InternalCalls.capsuleCollider_updateMaterial(id, ref value);
		}

		public Vector3 Offset
		{
			get
			{
				InternalCalls.capsuleCollider_getOffset(id, out Vector3 value);
				return value;
			}
			set => InternalCalls.capsuleCollider_setOffset(id, ref value);
		}

		public float Radius
		{
			get => InternalCalls.capsuleCollider_radius(id);
			set => InternalCalls.capsuleCollider_radius(id, value);
		}
		public float Height
		{
			get => InternalCalls.capsuleCollider_height(id);
			set => InternalCalls.capsuleCollider_height(id, value);
		}
		public bool Vertical
		{
			get => InternalCalls.capsuleCollider_vertical(id);
			set => InternalCalls.capsuleCollider_vertical(id, value);
		}
	}
}
