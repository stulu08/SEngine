#pragma warning disable 1591

namespace Stulu
{
	/// <summary>
	/// Internal collider class, does not work with get/has/add/remove component
	/// </summary>
	public interface Collider
	{
		/// <summary>
		/// Update the material of the shape attached to the collider
		/// </summary>
		PhysicsMaterial Material { get; set; }
	}
}
