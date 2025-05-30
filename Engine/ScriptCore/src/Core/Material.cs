
using System.Runtime.CompilerServices;

namespace Stulu
{
	/// <summary>
	/// Refrence to a material
	/// </summary>
	public class Material : AssetHandle
	{
		private Material(UUID id) : base(id) { }
		internal static Material Create(UUID id)
		{
			if (id == 0)
				return null;

			return new Material(id);
		}

		/// <summary>
		/// Name of the material
		/// </summary>
		public string Name => GetName();


		[MethodImpl(MethodImplOptions.InternalCall)]
		extern static string GetName();
	}
}
