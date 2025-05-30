
using System.Runtime.CompilerServices;

namespace Stulu
{
	/// <summary>
	/// Refrence to a mesh
	/// </summary>
	public class Mesh : AssetHandle {
		private Mesh(UUID id) : base(id){ }
		internal static Mesh Create(UUID id)
		{
			if (id == 0)
				return null;

			return new Mesh(id);
		}

		/// <summary>
		/// Name of the mesh
		/// </summary>
		public string Name => GetName();


		[MethodImpl(MethodImplOptions.InternalCall)]
		extern static string GetName();
	}
}
