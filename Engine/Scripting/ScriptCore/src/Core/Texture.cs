#pragma warning disable 1591
using System;

namespace Stulu {
	public abstract class Texture : AssetHandle {
		public virtual float width => 0;
		public virtual float height => 0;

	}
}
