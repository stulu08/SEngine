
using Stulu;

namespace Editor
{
	[InspectorRenderer(typeof(MeshFilterComponent), Icons.Cubes + " Mesh Filter", -42)]
	internal class MeshFilterInspector : Inspector
	{
		public override void Render(GameObject gameObject)
		{
			MeshFilterComponent meshFilter = gameObject.getComponent<MeshFilterComponent>();

			if (meshFilter == null) return;


			Mesh mesh = meshFilter.Mesh;
			if (ImGui.Mesh("Mesh", ref mesh))
			{
				meshFilter.Mesh = mesh;
			}

			if (mesh != null)
			{
				int count = mesh.SubMeshCount;
				if (count > 0 && ImGui.TreeNode($"{count} Sub-Meshes", ImGui.TreeNodeFlags.None))
				{
					ImGui.Indent();
					for (int i = 0; i < count; i++)
					{
						SubMeshDescription desc = mesh.GetSubMesh(i);
						ImGui.Text($"{desc.name} ({desc.indexCount} Indices)");
					}
					ImGui.Unindent();

					ImGui.TreePop();
				}
			}
		}
	}
}
