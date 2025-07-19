
using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Stulu
{
	/// <summary>
	/// Default vertext layout
	/// </summary>
	[StructLayout(LayoutKind.Sequential, Pack = 4, Size = 80)]
	public struct Vertex
	{
		Vector3 Position;
		Vector3 Normal;
		Vector3 TextureCoord;
		Vector4 Color;
		Vector4 Weights;
		Vector4I BoneIds;
	}
	/// <summary>
	/// Descripes a sub region of a mesh, use indexCount == 0 for removing the sub mesh
	/// </summary>
	public struct SubMeshDescription
	{
		/// <summary>
		/// Where this submesh's indices start in the global index buffer
		/// </summary>
		public uint indexOffset;
		/// <summary>
		/// How many indices it has (i.e., how many triangles * 3)
		/// </summary>
		public uint indexCount;
		/// <summary>
		/// Where its vertices start in the vertex buffer (for skinning or other per-submesh use)
		/// </summary>
		public uint vertexOffset;
		/// <summary>
		/// Name of the sub mesh
		/// </summary>
		public string name;
	};
	/// <summary>
	/// Refrence to a mesh
	/// </summary>
	public class Mesh : AssetHandle {
		private Mesh(UUID id, bool asWeakRef = false) : base(id, asWeakRef){ }
		internal static Mesh Create(UUID id)
		{
			if (id == 0)
				return null;

			return new Mesh(id);
		}
		internal static Mesh CreateWeak(UUID id)
		{
			if (id == 0)
				return null;

			return new Mesh(id, true);
		}

		/// <summary>
		/// Name of the mesh
		/// </summary>
		public string Name { get { return GetNameInternal(ID); } set { SetNameInternal(ID, value); } }

		/// <summary>
		/// Number of sub meshes defined
		/// </summary>
		public int SubMeshCount => GetSubMeshCountInternal(ID);

		/// <summary>
		/// Set the submesh
		/// </summary>
		public void SetSubMesh(int index, SubMeshDescription subMesh)
		{
			SetSubMeshInternal(ID, ref subMesh, index);
		}

		/// <summary>
		/// Get the submesh
		/// </summary>
		public SubMeshDescription GetSubMesh(int index)
		{
			GetSubMeshInternal(ID, out SubMeshDescription desc, index);
			return desc;
		}

		/// <summary>
		/// Removes a sub mesh
		/// </summary>
		public void RemoveSubMesh(int index)
		{
			SubMeshDescription desc = new SubMeshDescription
			{
				indexCount = 0
			};

			SetSubMeshInternal(ID, ref desc, index);
		}

		/// <summary>
		/// Raw access to the vertices
		/// </summary>
		public NativeArray<Vertex> Vertices { get { GetVerticesInternal(ID, out NativeArray<Vertex> val); return val; } set { SetVerticesInternal(ID, ref value); } }
		/// <summary>
		/// Raw access to the indices
		/// </summary>
		public NativeArray<uint> Indices { get { GetIndicesInternal(ID, out NativeArray<uint> val); return val; } set { SetIndicesInternal(ID, ref value); } }


		/// <summary>
		/// Calculates the bounds, suggested to always call after changing the mesh
		/// </summary>
		public void CalculateBounds() => CalculateBoundsInternal(ID);
		/// <summary>
		/// Calculates the normals based to the vertex positions
		/// </summary>
		public void CalculateNormals(bool reconstructMesh = false) => CalculateNormalsInternal(ID, reconstructMesh);
		/// <summary>
		/// Finalize any changes made to the mesh and push them to the buffers on the gpu, does not call CalculateBounds() !!!
		/// </summary>
		public void Construct() => CreateVertexMeshInternal(ID);


		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static string GetNameInternal(ulong ID);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void SetNameInternal(ulong ID, string name);


		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static int GetSubMeshCountInternal(ulong ID);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void GetSubMeshInternal(ulong ID, out SubMeshDescription mesh, int index);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void SetSubMeshInternal(ulong ID, ref SubMeshDescription mesh, int index = -1 /* all */);


		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void SetVerticesInternal(ulong ID, ref NativeArray<Vertex> vertices);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void GetVerticesInternal(ulong ID, out NativeArray<Vertex> vertices);


		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void SetIndicesInternal(ulong ID, ref NativeArray<uint> vertices);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void GetIndicesInternal(ulong ID, out NativeArray<uint> indices);


		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void CalculateBoundsInternal(ulong ID);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void CalculateNormalsInternal(ulong ID, bool upload);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void CreateVertexMeshInternal(ulong ID);
	}
}
