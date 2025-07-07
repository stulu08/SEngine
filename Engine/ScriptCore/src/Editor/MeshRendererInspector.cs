
using Stulu;

namespace Editor
{
	[InspectorRenderer(typeof(MeshRendererComponent), Icons.Pencil + " Mesh Renderer", -41)]
	internal class MeshRendererInspector : Inspector
	{
		public override void Render(GameObject gameObject)
		{
			MeshRendererComponent meshRenderer = gameObject.getComponent<MeshRendererComponent>();

			if (meshRenderer == null) return;



			Material material = meshRenderer.Material;
			if (ImGui.Material("Material", ref material))
			{
				meshRenderer.Material = material;
			}

			CullMode mode = meshRenderer.CullMode;
			if(ImGui.Combo("Cullmode", ref mode))
			{
				meshRenderer.CullMode = mode;
			}

		}
	}
}
