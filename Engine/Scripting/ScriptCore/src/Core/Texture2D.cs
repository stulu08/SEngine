#pragma warning disable 1591
using System;

namespace Stulu {
	public class Texture2D : Texture{
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

		public static bool operator ==(Texture2D tex1, Texture2D tex2) {
			return tex1.ID == tex2.ID;
		}
		public static bool operator !=(Texture2D tex1, Texture2D tex2) {
			return tex1.ID != tex2.ID;
		}
	}
}
