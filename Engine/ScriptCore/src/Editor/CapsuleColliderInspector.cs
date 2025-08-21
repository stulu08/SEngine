
using Stulu;

namespace Editor
{
	[InspectorRenderer(typeof(CapsuleColliderComponent), Icons.Database + " Capsule Collider", -38)]
	internal class CapsuleColliderInspector : Inspector
	{
		public override void Render(GameObject gameObject)
		{
			CapsuleColliderComponent collider = gameObject.getComponent<CapsuleColliderComponent>();

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

			float height = collider.Height;
			if (ImGui.Float("Height", ref height))
			{
				collider.Height = height;
			}

			float radius = collider.Radius;
			if (ImGui.Float("Radius", ref radius))
			{
				collider.Radius = radius;
			}
			

			bool vertical = collider.Vertical;
			if (ImGui.Bool("Vertical", ref vertical))
			{
				collider.Vertical = vertical;
			}
		}
	}
}
