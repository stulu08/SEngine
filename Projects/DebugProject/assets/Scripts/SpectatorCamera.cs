using Stulu;
using System.Collections;

public class SpectatorCamera : Component {
	[ShowInEditor]
	public float MoveSpeed = 8.0f;
	[ShowInEditor]
	public float Acceleration = 2.0f;
	[ShowInEditor]
	public float MouseVertical = 0.8f;
	[ShowInEditor]
	public float MouseHorizontal = 1.0f;
	[ShowInEditor]
	public float BulletForce = 50.0f;
	[ShowInEditor]
	public float FireRate = 20.0f;
	[ShowInEditor]
	public bool AutoFire = false;
	[ShowInEditor]
	public Texture2D Texture;

	KeyCode front = KeyCode.W;
	KeyCode back = KeyCode.S;
	KeyCode right = KeyCode.D;
	KeyCode left = KeyCode.A;
	KeyCode up = KeyCode.E;
	KeyCode down = KeyCode.Q;
	KeyCode accelerate = KeyCode.LeftShift;
	Vector3 mouse;

	int projectileCount = 0;
	float nextTimeToFire = 0;

	public override void onStart() {
		mouse = transform.eulerAngles;

		Log.Trace("Test");
		Log.Info("Test");
		Log.Warn("Test");
		Log.Error("Test");
		Log.Critical("Test");

	}
	public override void onUpdate() {
		Vector3 inputDiagonal = Input.getAxis(front, back) * transform.forward;
		Vector3 inputVertical = Input.getAxis(up, down) * Vector3.Up;
		Vector3 inputHorizonatl = Input.getAxis(right, left) * transform.right;

		Vector3 move = inputDiagonal + inputVertical + inputHorizonatl;
		transform.position += move * MoveSpeed * (Input.getKey(accelerate) ? Acceleration : 1.0f) * Time.deltaTime;

		mouse.y -= Input.getMouseDelta().x * (MouseHorizontal * Time.deltaTime);
		mouse.x -= Input.getMouseDelta().y * (MouseVertical * Time.deltaTime);
		transform.setRotation(Quaternion.Euler(mouse));

		if (!AutoFire ? Input.getMouseButtonDown(MouseButton.Left) : Input.getMouseButton(MouseButton.Left)) {
			Input.setCursorMode(CursorMode.Disabled);

			if (nextTimeToFire < Time.time) {
				GameObject projectile = GameObject.CreateSphere("Projectile_" + projectileCount, transform.position);
				RigidbodyComponent rb = projectile.addComponent<RigidbodyComponent>();
				rb.kinematic = false;
				rb.addForce(transform.forward * BulletForce, ForceMode.Impulse);

				projectile.addComponent<TestScript>().camera = this.gameObject;

				projectileCount++;
				nextTimeToFire = Time.time + (1.0f / FireRate);
			}
		}
	}

	public override void onDrawGizmos() {
		Gizmo.drawOutlinedCube(transform.position, Color.Cyan);
		if(Texture != null)
			Graphics2D.drawTexturedQuad(Texture, transform.position, Color.White);
	}
}