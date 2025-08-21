using Stulu;
using System.Collections;
using System.Reflection;
using System.Reflection.Emit;

public class TestScript : Component {
	[ShowInEditor]
	public GameObject camera;
	[ShowInEditor]
	public Vector4 color = Color.Cyan;

	public override void onAwake() {
		
	}
	public override void onStart() {
		
	}
	public override void onUpdate() {
	}
	public override void onRender2D() {
	}
	public override void onDrawGizmos() {
	if (camera != null)
	{
		Graphics2D.drawLine(transform.worldPosition, camera.transform.worldPosition, color);

		}
	}
}