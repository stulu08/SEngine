using Stulu;
using System.Collections;

public class TestScript : Component {

	RigidbodyComponent rigidbodyComponent;

	public override void onStart() {
		rigidbodyComponent = gameObject.getComponent<RigidbodyComponent>();
		if(rigidbodyComponent == null) {
			//we warn the user, that something happend, which can cause proplems
			Log.Warn("Warning: No RigidbodyComponent found!");
		}
	}
	public override void onUpdate() {
		if (Input.getKeyDown(KeyCode.Space)) {
			if (rigidbodyComponent == null) {
				//We say that a problem has occured
				Log.Error("RigidbodyComponent is null");
			} else {
				rigidbodyComponent.addForce(Vector3.Up, ForceMode.Impulse);
			}
		}

		if (Input.getMouseButtonDown(MouseButton.Left)) {
			Log.Trace("Calculating...");//Less important information

			Matrix4 mat1 = new Matrix4(
				new Vector4(8, 4, 7, 9),
				new Vector4(12, 7, 204, 1),
				new Vector4(7, 102, 3, 5),
				new Vector4(123, 7, 8, 2)
			);
			Matrix4 mat2 = new Matrix4(
				new Vector4(12, 7, 204, 1),
				new Vector4(8, 4, 7, 9),
				new Vector4(123, 7, 8, 2),
				new Vector4(7, 102, 3, 5)
			);
			Matrix4 mat3 = mat1 * mat2;
			Log.Info(mat3); // We can log any object by using the Object.ToString() function
		}
	}
}