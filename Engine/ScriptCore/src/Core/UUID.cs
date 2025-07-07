#pragma warning disable 1591
using System;
using System.Runtime.InteropServices;

namespace Stulu {
	[StructLayout(LayoutKind.Explicit, Size = 8)]
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

		[MarshalAs(UnmanagedType.U8)]
		[FieldOffset(0)]
		public UInt64 ID;
	}
}
