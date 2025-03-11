
using Stulu;

namespace Editor
{
	[InspectorRenderer(typeof(TransformComponent), "Transform")]
	internal class TransformInspector : Inspector
	{
		public override void Render(GameObject gameObject) {
			TransformComponent transform = gameObject.getComponent<TransformComponent>();

			if (transform == null) return;

			Vector3 position = transform.position;
			if(ImGui.Vector3("Position", ref position, 0.25f))
			{
				transform.position = position;
			}

			Vector3 rotation = transform.eulerAngles;
			if (ImGui.Vector3("Rotation", ref rotation, 1.0f))
			{
				transform.setRotation(new Quaternion(Mathf.Radians(rotation)));
			}

			Vector3 scale = transform.scale;
			if (ImGui.Vector3("Scale", ref scale, 0.1f))
			{
				transform.scale = scale;
			}
		}
	}
}
