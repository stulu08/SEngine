using Stulu;

namespace Editor
{
	/// <summary>
	/// Use this to define inspector behaviour for components
	/// </summary>
	public abstract class Inspector {
		/// <summary>
		/// This is called inside the inspector for rendering the components properties
		/// </summary>
		public abstract void Render(GameObject gameObject);
		/// <summary>
		/// This is called when the gameobject inside the scene editor gizmo pass while this gameobject is selected
		/// </summary>
		public virtual void RenderGizmos(GameObject gameObject) { }

		internal void Impl_Render(ulong goID)
		{
			GameObject gameObject = new GameObject(goID);
			Render(gameObject);
		}
		internal void Impl_RenderGizmos(ulong goID)
		{
			GameObject gameObject = new GameObject(goID);
			RenderGizmos(gameObject);
		}
	}
}
