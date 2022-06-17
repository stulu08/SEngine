using Stulu;
using System.Collections;

public class TestScript : Component {

	RigidbodyComponent rb;

	public override void onStart() {
		rb = null;
	}
	public override void onUpdate() {
		rb.addForce(Vector3.Forward,ForceMode.Impulse);
	}
}
