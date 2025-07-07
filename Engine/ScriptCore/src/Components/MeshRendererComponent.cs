

using System.Runtime.CompilerServices;

namespace Stulu
{
	/// <summary>
	/// The face to be drawn
	/// </summary>
	public enum CullMode
	{
		/// <summary>
		/// Back Face
		/// </summary>
		Back, 
		/// <summary>
		/// Front Face
		/// </summary>
		Front, 
		/// <summary>
		/// Both Faces!
		/// </summary>
		BackAndFront
	};

	/// <summary>
	/// This contains the material brrrrrrrrrrrrr
	/// </summary>
	public class MeshRendererComponent : GameObjectAttached
	{
		/// <summary>
		/// Base Material, GetMaterial(0)
		/// </summary>
		public Material Material { get => GetMaterial(0); set => SetMaterial(value, 0); }
		/// <summary>
		/// Set the material, index will be used for equivialant sub meshes
		/// </summary>
		public void SetMaterial(Material material, int index = 0) => SetMaterialInternal(id, material.ID, index);
		/// <summary>
		/// Get the material, index will be used for equivialant sub meshes
		/// </summary>
		public Material GetMaterial(int index = 0) => Material.CreateWeak(GetMaterialInternal(id, index));
		/// <summary>
		/// Set/Get the cull mode
		/// </summary>
		public CullMode CullMode { get => (CullMode)GetCullModeInternal(id); set => SetCullModeInternal(id, (uint)value); }


		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void SetMaterialInternal(ulong ID, ulong MaterialID, int index);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static ulong GetMaterialInternal(ulong ID, int index);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void SetCullModeInternal(ulong ID, uint cullMode);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static uint GetCullModeInternal(ulong ID);
	}
}
