using Stulu;
using System.Collections;

public class TestScript : Component {
	[ShowInEditor]
	public Texture2D texture;
	[ShowInEditor]
	public Vector3 vec;

	public override void onStart() {

	}
	public override void onUpdate() {
		if(texture != null) {
			Log.Info(Folders.Assets);
			Log.Info(texture.width + "|" + texture.height + "|" + texture.path);
		}
		
	}
	public override void onRender2D() {

	}
	public override void onDrawGizmos() {
		return;
		for(int x = 0; x < 10; x++) {
			for (int y = 0; y < 10; y++) {
				for (int z = 0; z < 10; z++) {
					Gizmo.drawSphere(new Vector3((float)x, (float)y, (float)z), new Vector4(new Vector3(x,y,z)/10.0f));
				}
			}
		}
	}
}