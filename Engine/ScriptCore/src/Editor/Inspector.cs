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

		internal void Impl_Render(ulong goID)
		{
			GameObject gameObject = new GameObject(goID);
			Render(gameObject);
		}
	}
}
