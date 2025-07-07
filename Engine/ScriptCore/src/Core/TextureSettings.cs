#pragma warning disable 1591

using System.Runtime.InteropServices;

namespace Stulu
{
	public enum TextureFormat /* : uint32_t */
	{
		RGBA = 0, RGB = 1, RG = 2, R = 3, 
		SRGB = 4, SRGBA = 5, 
		RGBA16F = 6, RGB16F = 7, RG16F = 21, R16F = 22, R32UI = 23,
		RGBA32F = 9, RGB32F = 19, RG32F = 8, R32F = 20,
		// Auto can only be used for files
		Auto = 10, None = 11,
		Depth16 = 12, Depth24 = 13, Depth32 = 14, Depth32F = 15, 
		Depth24_Stencil8 = 16, Depth32F_Stencil8 = 17, Stencil8 = 18,
	};
	public enum TextureWrap /* : uint32_t */
	{
		ClampToEdge = 0, 
		Repeat = 1, 
		ClampToBorder = 2
	};
	public enum TextureFiltering /* : uint32_t */
	{
		Linear,
		Nearest,
		Trilinear,
	};
	/*
	struct TextureSettings
	{
		TextureFormat format = TextureFormat::Auto;
		TextureWrap wrap = TextureWrap::ClampToEdge;
		glm::vec2 tiling = { 1.0f, 1.0f };
		uint32_t levels = 1;
		glm::vec4 border = glm::vec4(1.0f);
		TextureFiltering filtering = TextureFiltering::Linear;

		TextureSettings(TextureFormat format = TextureFormat::Auto, TextureWrap wrap = TextureWrap::ClampToEdge,
			const glm::vec2& tiling = { 1.0f,1.0f }, uint32_t levels = 1, glm::vec4 border = glm::vec4(1.0f),
			TextureFiltering filtering = TextureFiltering::Linear)
			:format(format), wrap(wrap), tiling(tiling), levels(levels), border(border), filtering(filtering) { }
	};
	*/

	[StructLayout(LayoutKind.Sequential, Size = 40, Pack = 8)]
	public struct TextureSettings
	{
		[MarshalAs(UnmanagedType.U4)]
		public TextureFormat Format;

		[MarshalAs(UnmanagedType.U4)]
		public TextureWrap Wrap;

		[MarshalAs(UnmanagedType.Struct)]
		public Vector2 Tiling;

		[MarshalAs(UnmanagedType.U4)]
		public uint Levels;

		[MarshalAs(UnmanagedType.Struct)]
		public Vector4 Borders;

		[MarshalAs(UnmanagedType.U4)]
		public TextureFiltering Filtering;

		public TextureSettings(TextureFormat format)
		{
			Format = format;
			Wrap = TextureWrap.ClampToEdge;
			Tiling = new Vector2(1.0f);
			Levels = 1;
			Borders = Vector4.Zero;
			Filtering = TextureFiltering.Linear;
		}
	}
}
