using Stulu;
using System.Collections;
public static class Layers {
	public static float Player = 50.0f;
	public static float Bullet = 40.0f;
}
public class Player : Component {
	public enum State {
		Walking, Shooting, Idle, Knfife, KnifeIdle, 
	};
	[ShowInEditor]
	public float Speed = 8.0f;
	[ShowInEditor]
	public float FireRate = 4.0f;
	public float nextTimeToFire = 0.0f;

	[ShowInEditor]
	public GameObject SpriteRendererObject;
	[ShowInEditor]
	public GameObject WeaponPosObject;

	[ShowInEditor]
	public Texture2D WalkTexture;
	[ShowInEditor]
	public Texture2D ShootTexture;
	[ShowInEditor]
	public Texture2D IdleTexture;
	[ShowInEditor]
	public Texture2D KnifeTexture;
	[ShowInEditor]
	public Texture2D KnifeIdle;

	[ShowInEditor]
	public Texture2D activeTexture;

	public State state;

	public override void onAwake() {
		transform.position = new Vector3(transform.position.x, transform.position.y, Layers.Player);
	}
	int counter = 0;
	public override void onUpdate() {
		Vector3 move = new Vector3(0, 0, 0);
		move.y += Input.getAxis(KeyCode.W, KeyCode.S);
		move.x += Input.getAxis(KeyCode.A, KeyCode.D);
		transform.position += move * Speed * Time.deltaTime;
		state = State.Idle;
		if (move.x != 0 || move.y != 0) {
			state = State.Walking;
		}
		else if (Input.getMouseButton(MouseButton.Left)) {
			// is shooting
			state = State.Shooting;
			if(Time.time > nextTimeToFire) {
				GameObject bullet = new GameObject("Bullet_" + counter.ToString(), WeaponPosObject.transform.worldPosition, WeaponPosObject.transform.worldRotation);
				bullet.addComponent<Bullet>();
				counter++;
				nextTimeToFire = Time.time + 1.0f / FireRate;
			}
			

		}
		UpdateTextureScale();
	}

	public void UpdateTextureScale() {
		if (SpriteRendererObject != null) {

			switch (state) {
				case State.Idle:
					SpriteRendererObject.transform.scale = new Vector3(1, 1, 1);
					activeTexture = IdleTexture;
					break;
				case State.Walking:
					SpriteRendererObject.transform.scale = new Vector3(1, 1, 1);
					activeTexture = WalkTexture;
					break;
				case State.Shooting:
					SpriteRendererObject.transform.scale = new Vector3(1, 2, 1);
					activeTexture = ShootTexture;
					break;
				case State.Knfife:
					SpriteRendererObject.transform.scale = new Vector3(1, 1, 1);
					activeTexture = KnifeTexture;
					break;
				case State.KnifeIdle:
					SpriteRendererObject.transform.scale = new Vector3(1, 1, 1);
					activeTexture = KnifeIdle;
					break;
			}
			if (SpriteRendererObject.hasComponent<SpriteRendererComponent>()) {
				SpriteRendererObject.getComponent<SpriteRendererComponent>().texture = activeTexture;
			}
		}
	}
}