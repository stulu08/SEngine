using System;

namespace Stulu {
	/// <summary>
	/// Refrence to an Texture in the Engine
	/// </summary>
	public abstract class Texture : AssetHandle {
		/// <summary>
		/// Texture Width
		/// </summary>
		public virtual float width => 0;
		/// <summary>
		/// Texture Height
		/// </summary>
		public virtual float height => 0;

	}
}
