using System;

namespace Stulu {
	/// <summary>
	/// Refrence to an Asset
	/// </summary>
	public class AssetHandle {
		internal void Initilize(UUID uuid) {
			this._id = uuid;
		}
		internal UUID GetUUID() {
			return this.ID;
		}
		/// <summary>
		/// The actual id
		/// </summary>
		public UUID ID => _id;
		internal UInt64 _id;
	}
}
