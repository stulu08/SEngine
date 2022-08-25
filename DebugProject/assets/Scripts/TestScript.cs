using Stulu;
using System.Collections;

public class TestScript : Component {
	public override void onStart() {
		
	}
	public override void onUpdate() {
		
	}
	public override void onRender2D() {

	}
	public override void onDrawGizmos() {
		for(int x = 0; x < 10; x++) {
			for (int y = 0; y < 10; y++) {
				for (int z = 0; z < 10; z++) {
					Gizmo.drawSphere(new Vector3((float)x, (float)y, (float)z), new Vector4(new Vector3(x,y,z)/10.0f));
				}
			}
		}
	}
}