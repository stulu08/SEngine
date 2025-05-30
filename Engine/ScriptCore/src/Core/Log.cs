namespace Stulu {
	/// <summary>
	/// Use it to log any data to the console while debugging
	/// </summary>
	public static class Log {
		internal enum Level {
			trace = 0, info = 2, warn = 3, error = 4, critical = 5
		};
		/// <summary>
		///Prints the message to the console as a critical message
		/// </summary>
		public static void Critical(string message) {
			InternalCalls.log_client_log((int)Level.critical, message);
		}
		/// <summary>
		/// Prints the message to the console as a error message
		/// </summary>
		public static void Error(string message) {
			InternalCalls.log_client_log((int)Level.error, message);
		}
		/// <summary>
		/// Prints the message to the console as a warn message
		/// </summary>
		public static void Warn(string message) {
			InternalCalls.log_client_log((int)Level.warn, message);
		}
		/// <summary>
		/// Prints the message to the console as a info message
		/// </summary>
		public static void Info(string message) {
			InternalCalls.log_client_log((int)Level.info, message);
		}
		/// <summary>
		/// Prints the message to the console as a trace message
		/// </summary>
		public static void Trace(string message) {
			InternalCalls.log_client_log((int)Level.trace, message);
		}
		/// <summary>
		/// Convertes the object to a string using Object.ToString and prints it to the console as a critical message
		/// </summary>
		public static void Critical(object message) {
			InternalCalls.log_client_log((int)Level.critical, message.ToString());
		}
		/// <summary>
		/// Convertes the object to a string using Object.ToString and prints it to the console as a error message
		/// </summary>
		public static void Error(object message) {
			InternalCalls.log_client_log((int)Level.error, message.ToString());
		}
		/// <summary>
		/// Convertes the object to a string using Object.ToString and prints it to the console as a warn message
		/// </summary>
		public static void Warn(object message) {
			InternalCalls.log_client_log((int)Level.warn, message.ToString());
		}
		/// <summary>
		/// Convertes the object to a string using Object.ToString and prints it to the console as a info message
		/// </summary>
		public static void Info(object message) {
			InternalCalls.log_client_log((int)Level.info, message.ToString());
		}
		/// <summary>
		/// Convertes the object to a string using Object.ToString and prints it to the console as a trace message
		/// </summary>
		public static void Trace(object message) {
			InternalCalls.log_client_log((int)Level.trace, message.ToString());
		}


		internal static void EngineCritical(string message) {
			InternalCalls.log_engine_log((int)Level.critical, message);
		}
		internal static void EngineError(string message) {
			InternalCalls.log_engine_log((int)Level.error, message);
		}
		internal static void EngineWarn(string message) {
			InternalCalls.log_engine_log((int)Level.warn, message);
		}
		internal static void EngineInfo(string message) {
			InternalCalls.log_engine_log((int)Level.info, message);
		}
		internal static void EngineTrace(string message) {
			InternalCalls.log_engine_log((int)Level.trace, message);
		}
		internal static void EngineCritical(object message) {
			InternalCalls.log_engine_log((int)Level.critical, message.ToString());
		}
		internal static void EngineError(object message) {
			InternalCalls.log_engine_log((int)Level.error, message.ToString());
		}
		internal static void EngineWarn(object message) {
			InternalCalls.log_engine_log((int)Level.warn, message.ToString());
		}
		internal static void EngineInfo(object message) {
			InternalCalls.log_engine_log((int)Level.info, message.ToString());
		}
		internal static void EngineTrace(object message) {
			InternalCalls.log_engine_log((int)Level.trace, message.ToString());
		}
	}
}
