
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



			if (ImGui.TreeNode("Materials", ImGui.TreeNodeFlags.DefaultOpen)) {

				ImGui.Indent();

				int materialCount = meshRenderer.MaterialCount;
				if(ImGui.Int("Count", ref materialCount, 1))
					meshRenderer.MaterialCount = materialCount;	

				for (int i = 0; i < materialCount; i++)
				{
					Material material = meshRenderer.GetMaterial(i);
					if (ImGui.Material("Material " + i, ref material))
						meshRenderer.SetMaterial(material, i);
				}

				ImGui.Unindent();

				ImGui.TreePop();
			}


			CullMode mode = meshRenderer.CullMode;
			if(ImGui.Combo("Cullmode", ref mode))
			{
				meshRenderer.CullMode = mode;
			}

		}
	}
}
