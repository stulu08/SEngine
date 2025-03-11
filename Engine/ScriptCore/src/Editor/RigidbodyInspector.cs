
using Stulu;

namespace Editor
{
	[InspectorRenderer(typeof(RigidbodyComponent), Icons.Rocket + " Rigidbody")]
	internal class RigidbodyInspector : Inspector
	{
		public override void Render(GameObject gameObject)
		{
			RigidbodyComponent rigidbody = gameObject.getComponent<RigidbodyComponent>();

			if (rigidbody == null) return;
			
			bool kinematic = rigidbody.kinematic;
			if (ImGui.Bool("Mass", ref kinematic))
			{
				rigidbody.kinematic = kinematic;
			}
			bool useGravity = rigidbody.useGravity;
			if (ImGui.Bool("Use Gravity", ref useGravity))
			{
				rigidbody.useGravity = useGravity;
			}

			bool retainAccel = rigidbody.retainAccelaration;
			if (ImGui.Bool("Retain Accelaration", ref retainAccel))
			{
				rigidbody.retainAccelaration = retainAccel;
			}

			Vector3 massCenter = rigidbody.massCenterPos;
			if (ImGui.Vector3("Mass Center", ref massCenter))
			{
				rigidbody.massCenterPos = massCenter;
			}

			float mass = rigidbody.mass;
			if (ImGui.Float("Mass", ref mass, 0.0f))
			{
				rigidbody.mass = mass;
			}
		}
	}
}
