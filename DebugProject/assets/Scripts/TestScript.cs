using Stulu;
using System.Collections;

public class TestScript : Component {

	KeyCode front = KeyCode.Up;
	KeyCode back = KeyCode.Down;
	KeyCode right = KeyCode.Right;
	KeyCode left = KeyCode.Left;
	KeyCode up = KeyCode.PageUp;
	KeyCode down = KeyCode.PageDown;

	float oneG = 9.81f;

	public override void onStart() {
	}
	public override void onUpdate() {
		
		Vector3 inputDiagonal = Input.getAxis(front, back) * transform.forward;
		Vector3 inputVertical = Input.getAxis(up, down) * transform.up;
		Vector3 inputHorizonatl = Input.getAxis(right, left) * transform.right;

		Vector3 move = inputHorizonatl + inputDiagonal + inputVertical;


		transform.position += move * 1.0f * oneG * Time.deltaTime;
	}
}
