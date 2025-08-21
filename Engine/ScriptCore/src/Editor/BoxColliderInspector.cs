
using Stulu;

namespace Editor
{
	[InspectorRenderer(typeof(BoxColliderComponent), Icons.Cube + " Box Collider", -38)]
	internal class BoxColliderInspector : Inspector
	{
		public override void Render(GameObject gameObject)
		{
			BoxColliderComponent collider = gameObject.getComponent<BoxColliderComponent>();

			if (collider == null) return;

			Vector3 offset = collider.Offset;
			if (ImGui.Vector3("Offset", ref offset))
			{
				collider.Offset = offset;
			}

			if (ImGui.TreeNode("Material", ImGui.TreeNodeFlags.SpanAvailWidth))
			{
				PhysicsMaterial material = collider.Material;

				bool changed = false;

				changed |= ImGui.Float("Static Friction", ref material.StaticFriction, 0.1f);
				changed |= ImGui.Float("Dynamic Friction", ref material.DynamicFriction, 0.1f);
				changed |= ImGui.Float("Restitution", ref material.Restitution, 0.1f);

				if (changed)
				{
					collider.Material = material;
				}
				ImGui.TreePop();
			}

			Vector3 size = collider.Size;
			if (ImGui.Vector3("Size", ref size))
			{
				collider.Size = size;
			}
		}
	}
}
