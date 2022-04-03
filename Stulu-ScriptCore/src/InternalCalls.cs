using System;
using System.Runtime.CompilerServices;

namespace Stulu {
	public class InternalCalls {
		//Stulu/Core/Application.h
		[MethodImpl(MethodImplOptions.InternalCall)]
		public extern static void application_exit(int code);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public extern static uint application_getWidth();
		[MethodImpl(MethodImplOptions.InternalCall)]
		public extern static uint application_getHeight();

		//Stulu/Core/Log.h
		[MethodImpl(MethodImplOptions.InternalCall)]
		public extern static void log_engine_log(int level, string message);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public extern static void log_client_log(int level, string message);

		//Stulu/Core/Time.h
		[MethodImpl(MethodImplOptions.InternalCall)]
		public extern static float time_getFrameTime();
		[MethodImpl(MethodImplOptions.InternalCall)]
		public extern static float time_getDeltaTime();
		[MethodImpl(MethodImplOptions.InternalCall)]
		public extern static float time_getApplicationRuntime();
		[MethodImpl(MethodImplOptions.InternalCall)]
		public extern static float time_getScale();
		[MethodImpl(MethodImplOptions.InternalCall)]
		public extern static float time_getTime();


		//Stulu/Scene/GameObject.h
		//todo: find out how
		[MethodImpl(MethodImplOptions.InternalCall)]
		public extern static void gameObject_add_component(uint entityId, Type t);
	}
}
