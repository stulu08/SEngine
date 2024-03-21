using System;

namespace Stulu {
	/// <summary>
	/// Refrence to a 2D texture
	/// </summary>
	public class Texture2D : Texture {
		/// <summary>
		/// 1x1 White Texture
		/// </summary>
		public static Texture2D White => new Texture2D(InternalCalls.texture2d_getWhiteTexture());
		/// <summary>
		/// 1x1 Black Texture
		/// </summary>
		public static Texture2D Black => new Texture2D(InternalCalls.texture2d_getBlackTexture());

		internal Texture2D() {
			Initilize(0);
		}
		internal Texture2D(UInt64 id) {
			Initilize(id);
		}
		/// <summary>
		/// Texture Width
		/// </summary>
		public override float width => InternalCalls.texture2d_getWidth(ID);
		/// <summary>
		/// Texture Height
		/// </summary>
		public override float height => InternalCalls.texture2d_getHeight(ID);
		/// <summary>
		/// Path to the texture file, empty or null if created without file
		/// </summary>
		public string path => InternalCalls.texture2d_getPath(ID);

		/// <summary>
		/// Relative to the assets folder, for Editor {ProjDir}/assets and for Runtime {appname}-data/assets
		/// </summary>
		public static Texture2D FromPath(string path) {
			return new Texture2D(InternalCalls.texture2d_findbypath(path));
		}
	}
}
