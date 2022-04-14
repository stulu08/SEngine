using System;
using System.Collections;

using Stulu;
using Stulu.Editor;

public class EditorTest : Component {
	public Vector3 move = new Vector3(1.0f,2.0f,-3.3f);
	public float speed = 1.0f;
	public override void onAwake() {
		Log.Info("move: " + move);
		Log.Info("speed: " + speed);
	}
}