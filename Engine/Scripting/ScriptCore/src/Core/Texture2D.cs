#pragma warning disable 1591
using System;

namespace Stulu {
	public class Texture2D : Texture {
		public static Texture2D White => new Texture2D(InternalCalls.texture2d_getWhiteTexture());
		public static Texture2D Black => new Texture2D(InternalCalls.texture2d_getBlackTexture());

		internal Texture2D() {
			Initilize(0);
		}
		internal Texture2D(UInt64 id) {
			Initilize(id);
		}

		public override float width => InternalCalls.texture2d_getWidth(ID);
		public override float height => InternalCalls.texture2d_getHeight(ID);
		public string path => InternalCalls.texture2d_getPath(ID);

		/// <summary>
		/// Relative to the assets folder, for Editor {ProjDir}/assets and for Runtime {appname}-data/assets
		/// </summary>
		public static Texture2D FromPath(string path) {
			return new Texture2D(InternalCalls.texture2d_findbypath(path));
		}
	}
}
