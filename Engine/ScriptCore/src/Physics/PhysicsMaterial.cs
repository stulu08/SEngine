#pragma warning disable 1591
using System;

namespace Stulu
{
	public struct PhysicsMaterial {
		/// <summary>
		/// Coefficient of static friction
		/// </summary>
		public float StaticFriction;
		/// <summary>
		/// Coefficient of dynamic friction
		/// </summary>
		public float DynamicFriction;
		/// <summary>
		/// Coefficient of restitution
		/// </summary>
		public float Restitution;

		public PhysicsMaterial(float sF = 0.6f, float dF = 0.6f, float re = 0.0f)
		{
			StaticFriction = sF;
			DynamicFriction = dF;
			Restitution = re;
		}
	}
}