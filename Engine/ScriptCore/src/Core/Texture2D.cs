using System;
using System.Runtime.CompilerServices;

namespace Stulu {
	/// <summary>
	/// Refrence to a 2D texture
	/// </summary>
	public class Texture2D : Texture {
		private Texture2D(UUID id, bool asWeakRef = false) : base(id, asWeakRef) { textureSettingsCache.Format = TextureFormat.None; }

		private TextureSettings textureSettingsCache = new TextureSettings(TextureFormat.None);

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
		/// Creates a Texture2D from an asset handle, no ownership tracking
		/// </summary>
		internal static Texture2D CreateWeak(UUID id)
		{
			if (id == 0)
				return null;

			return new Texture2D(id, true);
		}


		/// <summary>
		/// Settings for the texture target
		/// </summary>
		public ref TextureSettings Settigns
		{
			get
			{
				if (textureSettingsCache.Format == TextureFormat.None)
				{
					GetTextureSettings(ID, out textureSettingsCache);
				}
				return ref textureSettingsCache;
			}
		}

		/// <summary>
		/// Updates the changes made to Settings, changing the format results in reloading the tetxure from the file, format change may be reverted or set to a more suitable one
		/// </summary>
		public void UpdateTextureSettings()
		{
			SetTextureSettings(ID, ref Settigns);
		}

		/// <summary>
		/// 1x1 White Texture
		/// </summary>
		public static Texture2D White => Create(GetWhiteTexture());
		/// <summary>
		/// 1x1 Black Texture
		/// </summary>
		public static Texture2D Black => Create(GetBlackTexture());
		/// <summary>
		/// Relative to the assets folder, for Editor {ProjDir}/assets and for Runtime {appname}-data/assets
		/// </summary>
		public static Texture2D FromPath(string path)
		{
			return new Texture2D(Findbypath(path));
		}



		/// <summary>
		/// Texture Width
		/// </summary>
		public override float Width => GetWidth(ID);
		/// <summary>
		/// Texture Height
		/// </summary>
		public override float Height => GetHeight(ID);

		

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static float GetWidth(ulong textureID);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static float GetHeight(ulong textureID);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static string GetPath(ulong textureID);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static ulong Findbypath(string path);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static ulong GetWhiteTexture();
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static ulong GetBlackTexture();

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void GetTextureSettings(ulong id, out TextureSettings settings);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void SetTextureSettings(ulong id, ref TextureSettings settings);
	}
}
