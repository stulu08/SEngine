#pragma warning disable 1591
using System;

namespace Stulu {
	public struct UUID {
		public UUID(UInt64 id) {
			this.ID = id;
		}

		public static implicit operator UInt64(UUID uuid) {
			return uuid.ID;
		}
		public static implicit operator UUID(UInt64 id) {
			return new UUID(id);
		}
		public override string ToString() {
			return ID.ToString();
		}

		UInt64 ID;
	}
}
