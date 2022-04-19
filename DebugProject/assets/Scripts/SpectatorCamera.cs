using System;
using System.Collections;

using Stulu;

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


	[ShowInEditor]
	Vector3 mouse;
	public override void onStart() {
		mouse.x = transform.eulerRotation.x;
		mouse.y = transform.eulerRotation.y;

	}
	public override void onUpdate() {
		Vector3 inputDiagonal = Input.getAxis(front,back) * transform.forward;
		Vector3 inputVertical = Input.getAxis(up, down) * Vector3.Up;
		Vector3 inputHorizonatl = Input.getAxis(right, left) * transform.right;

		Vector3 move = inputDiagonal + inputVertical + inputHorizonatl;

		transform.position += move * moveSpeed * (Input.isKeyDown(accelerate) ? accelerationMultiplier : 1.0f) * Time.deltaTime;

		mouse.x += Input.getMouseDelta().x * (mouseSensitivityVertical * Time.deltaTime);
		mouse.y -= Input.getMouseDelta().y * (mouseSensitivityHorizontal * Time.deltaTime);

		transform.setRotation(new Quaternion(new Vector3(.0f, mouse.x, mouse.y)));

		if (Input.isKeyDown(KeyCode.Escape))
			Input.setCursorMode(CursorMode.Normal);
		else if(Input.getCursorMode() != CursorMode.Disabled) {
			Input.setCursorMode(CursorMode.Disabled);
		}
	}
}
