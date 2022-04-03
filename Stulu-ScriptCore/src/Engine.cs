using System;
using System.Collections;

namespace Stulu {
    public class Engine {
        float t = .0f;
        public Engine(float f) {
            t = f;
        }

        public void onUpdate() {
            t += Time.frameTime;
            if (t > 2.0f) {
                Log.Info("Frametime: " + Time.frameTime);
                Log.Info("Deltatime: " + Time.deltaTime);
                Log.Info("Runtime: " + Time.applicationRuntime);
                Log.Info("Time: " + Time.time);
                Log.Info("Scale: " + Time.Scale);
                t = .0f;
            }
		}
    }
}
