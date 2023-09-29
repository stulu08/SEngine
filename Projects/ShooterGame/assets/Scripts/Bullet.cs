using Stulu;
using System.Collections;
public class Bullet : Component {
	[ShowInEditor]
	public float speed = 15.0f;
	[ShowInEditor]
	public float lifetime = 0.0f;
	[ShowInEditor]
	public float maxLifetime = 5.0f;

	SpriteRendererComponent spriteRenderer;
	Texture2D bulletTexture;
	public override void onAwake() {
		spriteRenderer = gameObject.addComponent<SpriteRendererComponent>();
		bulletTexture = Texture2D.FromPath("Textures/player/bullet.png");
		if(bulletTexture != null && spriteRenderer!= null ) {
			spriteRenderer.texture = bulletTexture;
		}
		transform.position = new Vector3(transform.position.x, transform.position.y, Layers.Bullet);
		transform.scale = new Vector3(0.5f, 1.0f, 1.0f) / 4.0f;
	}
	public override void onUpdate() {
		if(lifetime > maxLifetime) {
			GameObject.Destroy(gameObject);
		}
		else {
			transform.position +=  transform.up * speed * Time.deltaTime;
			lifetime += Time.deltaTime;
		}
	}
}