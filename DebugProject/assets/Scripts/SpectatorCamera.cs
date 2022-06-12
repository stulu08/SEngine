using Stulu;
using System.Collections;

public class SpectatorCamera : Component {
	[ShowInEditor]
	public float moveSpeed = 8.0f;
	[ShowInEditor]
	public float accelerationMultiplier = 2.0f;
	[ShowInEditor]
	public float mouseSensitivityVertical = 1.0f;
	[ShowInEditor]
	public float mouseSensitivityHorizontal = .8f;


	KeyCode front = KeyCode.W;
	KeyCode back = KeyCode.S;
	KeyCode right = KeyCode.D;
	KeyCode left = KeyCode.A;
	KeyCode up = KeyCode.E;
	KeyCode down = KeyCode.Q;

	KeyCode accelerate = KeyCode.LeftShift;

	Vector3 mouse;


	public override void onStart() {
		Input.setCursorMode(CursorMode.Disabled);
		mouse = transform.eulerAngles;

	}
	public override void onUpdate() {
		Vector3 inputDiagonal = Input.getAxis(front, back) * transform.forward;
		Vector3 inputVertical = Input.getAxis(up, down) * Vector3.Up;
		Vector3 inputHorizonatl = Input.getAxis(right, left) * transform.right;

		Vector3 move = inputDiagonal + inputVertical + inputHorizonatl;

		transform.position += move * moveSpeed * (Input.getKey(accelerate) ? accelerationMultiplier : 1.0f) * Time.deltaTime;

		mouse.y += Input.getMouseDelta().x * (mouseSensitivityVertical * Time.deltaTime);
		mouse.z -= Input.getMouseDelta().y * (mouseSensitivityHorizontal * Time.deltaTime);
		transform.setRotation(new Quaternion(mouse));

		if (Input.getKeyDown(KeyCode.Escape)) {
			if (Input.getCursorMode() == CursorMode.Disabled)
				Input.setCursorMode(CursorMode.Normal);
			else
				Input.setCursorMode(CursorMode.Disabled);
		}
	}
}