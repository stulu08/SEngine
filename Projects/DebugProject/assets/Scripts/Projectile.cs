using Stulu;
using System.Collections;

public class Projectile : Component {

	[ShowInEditor]
	public float force = 9.81f;
	[ShowInEditor]
	public Vector3 direction = new Vector3(0, 1, 0);

	RigidbodyComponent rigidbodyComponent;
	public override void onStart() {
		rigidbodyComponent = gameObject.getComponent<RigidbodyComponent>();
	}
	public override void onUpdate() {
		if (Input.getKeyDown(KeyCode.R)) {
			rigidbodyComponent.addForce(force*direction, ForceMode.Impulse);
		}
	}
}