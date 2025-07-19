using System.Runtime.CompilerServices;

namespace Stulu
{
	/// <summary>
	/// This contains the mesh
	/// </summary>
	public class MeshFilterComponent : GameObjectAttached
	{
		/// <summary>
		/// Base Material, GetMaterial(0)
		/// </summary>
		public Mesh Mesh { get => Mesh.CreateWeak(GetMeshInternal(id)); set => SetMeshInternal(id, AssetHandle.GetIDSafe(value)); }

		
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void SetMeshInternal(ulong ID, ulong MeshID);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static ulong GetMeshInternal(ulong ID);
	}
}
