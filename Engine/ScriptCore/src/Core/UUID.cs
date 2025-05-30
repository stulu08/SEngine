#pragma warning disable 1591
using System;

namespace Stulu {
	public struct UUID {
		public static UUID Null = 0;

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

		public UInt64 ID;
	}
}
