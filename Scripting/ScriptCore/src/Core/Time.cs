namespace Stulu {
	/// <summary>
	/// You can acces time information with it
	/// </summary>
	public static class Time {
		/// <summary>
		/// The duration of the last update in seconds
		/// </summary>
		static public float frameTime => InternalCalls.time_getFrameTime();
		/// <summary>
		/// The duration of the last update in seconds scaled by Time.Scale
		/// </summary>
		static public float deltaTime => InternalCalls.time_getDeltaTime();
		/// <summary>
		/// The time passed since the scene started
		/// </summary>
		static public float time => InternalCalls.time_getTime();
		/// <summary>
		/// The time passed since the application started
		/// </summary>
		static public float applicationRuntime => InternalCalls.time_getApplicationRuntime();
		/// <summary>
		/// The Scale how fast time passes, affects only Time.deltaTime
		/// </summary>
		static public float Scale { get => InternalCalls.time_getScale(); set => InternalCalls.time_setScale(value); }
	}
}
