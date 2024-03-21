using Stulu;
using System.Collections;
using System.Reflection;
using System.Reflection.Emit;

public class TestScript : Component {
	[ShowInEditor]
	public Texture2D texture;
	[ShowInEditor]
	public Vector4 color = Color.Cyan;

	public override void onAwake() {
		
	}
	public override void onStart() {
		
	}
	public override void onUpdate() {
	}
	public override void onRender2D() {
		//if (camera != null) {
		//	Graphics2D.drawLine(transform.worldPosition, camera.transform.worldPosition, color);
		//}
	}
	public override void onDrawGizmos() {

	}
}