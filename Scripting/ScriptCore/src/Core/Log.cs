namespace Stulu {
	public static class Log {
		public enum Level {
			trace = 0, info = 1, warn = 2, error = 3, critical = 4
		};
		public static void Critical(string message) {
			InternalCalls.log_client_log((int)Level.critical, message);
		}
		public static void Error(string message) {
			InternalCalls.log_client_log((int)Level.error, message);
		}
		public static void Warn(string message) {
			InternalCalls.log_client_log((int)Level.warn, message);
		}
		public static void Info(string message) {
			InternalCalls.log_client_log((int)Level.info, message);
		}
		public static void Trace(string message) {
			InternalCalls.log_client_log((int)Level.trace, message);
		}
		public static void Critical(object message) {
			InternalCalls.log_client_log((int)Level.critical, message.ToString());
		}
		public static void Error(object message) {
			InternalCalls.log_client_log((int)Level.error, message.ToString());
		}
		public static void Warn(object message) {
			InternalCalls.log_client_log((int)Level.warn, message.ToString());
		}
		public static void Info(object message) {
			InternalCalls.log_client_log((int)Level.info, message.ToString());
		}
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
