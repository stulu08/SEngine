namespace Stulu {
	public static class Time {
		static public float frameTime => InternalCalls.time_getFrameTime();
		//frame time scaled by Scale
		static public float deltaTime => InternalCalls.time_getDeltaTime();
		//time since scene runtime started
		static public float time => InternalCalls.time_getTime();
		//time since application startup
		static public float applicationRuntime => InternalCalls.time_getApplicationRuntime();
		//deltatime is multiplied with this but not runtime
		static public float Scale { get => InternalCalls.time_getScale(); set => InternalCalls.time_setScale(value); }
	}
}
