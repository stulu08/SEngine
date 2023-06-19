#pragma warning disable 1591
using System;

namespace Stulu {
	public class AssetHandle {
		internal void Initilize(UUID uuid) {
			this.ID = uuid;
		}
		internal UUID GetUUID() {
			return this.ID;
		}

		public UUID ID;
	}
}
