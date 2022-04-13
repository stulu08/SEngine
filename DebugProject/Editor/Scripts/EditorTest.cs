using System;
using System.Collections;

using Stulu;
using Stulu.Editor;

public class EditorTest : Component {
	public Vector3 move = new Vector3(1.0f,2.0f,-3.3f);
	public float speed = 1.0f;
	public int inttt;
	public override void onAwake() {
		Log.Info("move: " + move);
		Log.Info("speed: " + speed);
		Log.Info("inttt: " + inttt);
	}
	public override void onDrawEditorGUI() {
		if (Gui.Begin("Test")) {
			Gui.Vector3("Vector", ref move);
			speed = Gui.Float("Float", speed);
			inttt = Gui.SliderInt("int", inttt, 0,100);
		}
		Gui.End();
	}
}