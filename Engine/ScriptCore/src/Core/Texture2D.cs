using System;

namespace Stulu {
	/// <summary>
	/// Refrence to a 2D texture
	/// </summary>
	public class Texture2D : Texture {
		private Texture2D(UUID id) : base(id) { }

		/// <summary>
		/// Creates a Texture2D from an asset handle, null if invalid id
		/// </summary>
		internal static Texture2D Create(UUID id)
		{
			if (id == 0)
				return null;

			return new Texture2D(id);
		}

		/// <summary>
		/// 1x1 White Texture
		/// </summary>
		public static Texture2D White => new Texture2D(InternalCalls.texture2d_getWhiteTexture());
		/// <summary>
		/// 1x1 Black Texture
		/// </summary>
		public static Texture2D Black => new Texture2D(InternalCalls.texture2d_getBlackTexture());
		/// <summary>
		/// Texture Width
		/// </summary>
		public override float Width => InternalCalls.texture2d_getWidth(ID);
		/// <summary>
		/// Texture Height
		/// </summary>
		public override float Height => InternalCalls.texture2d_getHeight(ID);

		/// <summary>
		/// Relative to the assets folder, for Editor {ProjDir}/assets and for Runtime {appname}-data/assets
		/// </summary>
		public static Texture2D FromPath(string path) {
			return new Texture2D(InternalCalls.texture2d_findbypath(path));
		}
	}
}
