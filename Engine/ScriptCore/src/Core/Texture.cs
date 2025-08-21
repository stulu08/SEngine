using System;

namespace Stulu {
	/// <summary>
	/// Refrence to an Texture in the Engine
	/// </summary>
	public abstract class Texture : AssetHandle {
		internal Texture(UUID id, bool asWeakRef = false) : base(id, asWeakRef) {}

		/// <summary>
		/// Texture Width
		/// </summary>
		public virtual float Width => 0;
		/// <summary>
		/// Texture Height
		/// </summary>
		public virtual float Height => 0;

	}
}
