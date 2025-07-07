
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

		}
	}
}
