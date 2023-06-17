using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stulu {
	/// <summary>
	/// You can acces important application data with
	/// </summary>
	public static class Application {
		/// <summary>
		/// The width of the main window
		/// </summary>
		public static uint width => InternalCalls.application_getWidth();
		/// <summary>
		/// The height of the main window
		/// </summary>
		public static uint height => InternalCalls.application_getHeight();
		/// <summary>
		/// The aspectRatio of the main window (width/height)
		/// </summary>
		public static float aspectRatio => (float)width / (float)height;
		/// <summary>
		/// Exits the Application, exits scene and closes window
		/// </summary>
		public static void exit(int code = 0) { InternalCalls.application_exit(code); }
	}
}
