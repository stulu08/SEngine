using System;

namespace Stulu {
	public static class Folders {
		/// <summary>
		/// Path to assets folder
		/// </summary>
		public static string Assets => InternalCalls.folders_assetPath();
	}
}
