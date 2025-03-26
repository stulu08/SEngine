
using Stulu;

namespace Editor
{
	[InspectorRenderer(typeof(RigidbodyComponent), Icons.Rocket + " Rigidbody", -39)]
	internal class RigidbodyInspector : Inspector
	{
		public override void Render(GameObject gameObject)
		{
			RigidbodyComponent rigidbody = gameObject.getComponent<RigidbodyComponent>();

			if (rigidbody == null) return;
			
			bool kinematic = rigidbody.kinematic;
			if (ImGui.Bool("Kinematic", ref kinematic))
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

			if (ImGui.TreeNode("Restraints", ImGui.TreeNodeFlags.FramePadding))
			{
				{
					bool aX = rigidbody.LockAngularVelocityX;
					bool aY = rigidbody.LockAngularVelocityY;
					bool aZ = rigidbody.LockAngularVelocityZ;
					if (ImGui.Bool("Angular X", ref aX))
					{
						rigidbody.LockAngularVelocityX = aX;
					}
					if (ImGui.Bool("Angular Y", ref aY))
					{
						rigidbody.LockAngularVelocityY = aY;
					}
					if (ImGui.Bool("Angular Z", ref aZ))
					{
						rigidbody.LockAngularVelocityZ = aZ;
					}
				}
				

				{
					bool lX = rigidbody.LockLinearVelocityX;
					bool lY = rigidbody.LockLinearVelocityY;
					bool lZ = rigidbody.LockLinearVelocityZ;
					if (ImGui.Bool("Linear X", ref lX))
					{
						rigidbody.LockLinearVelocityX = lX;
					}
					if (ImGui.Bool("Linear Y", ref lY))
					{
						rigidbody.LockLinearVelocityY = lY;
					}
					if (ImGui.Bool("Linear Z", ref lZ))
					{
						rigidbody.LockLinearVelocityZ = lZ;
					}
				}
				

				ImGui.TreePop();	
			}
		}
	}
}
